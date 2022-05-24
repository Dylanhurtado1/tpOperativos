#include "planificador.h"
#include "temporizador.h"

void iniciar_planificador_corto_plazo() {
	pthread_mutex_init(&mutex_ready, NULL);
	pthread_mutex_init(&mutex_blocked, NULL);
	pthread_mutex_init(&mutex_exec, NULL);
	sem_init(&sem_ready, 0, 0);
	sem_init(&sem_exec, 0, 0);
	sem_init(&sem_blocked, 0, 0);
	cola_ready = queue_create();
	cola_exec = queue_create();
	cola_blocked = queue_create();
	lista_blocked = list_create();
	pthread_create(&thread_ready, NULL, (void *)estado_ready, NULL);
	pthread_create(&thread_exec, NULL, (void *)estado_exec, NULL);
	pthread_create(&thread_blocked, NULL, (void *)estado_blocked, NULL);
	pthread_detach(thread_ready);
	pthread_detach(thread_exec);
	pthread_detach(thread_blocked);

	pthread_mutex_init(&mutex_io, NULL);
	pthread_mutex_init(&mutex_suspend, NULL);
	iniciar_timer();
}

void estado_ready(void *data) {
	while(1) {
		sem_wait(&sem_ready);

		pthread_mutex_lock(&mutex_ready);
		t_pcb *proceso = queue_pop(cola_ready);
		pthread_mutex_unlock(&mutex_ready);

		pthread_mutex_lock(&mutex_exec);
		queue_push(cola_exec, proceso);
		pthread_mutex_unlock(&mutex_exec);

		sem_post(&sem_exec);
	}
}

void estado_exec(void *data) {
	while(1) {
		sem_wait(&sem_exec);
		pthread_mutex_lock(&mutex_exec);
		t_pcb *proceso = queue_pop(cola_exec);
		pthread_mutex_unlock(&mutex_exec);
		enviar_proceso_a_cpu(proceso, socket_cpu_dispatch);
		eliminar_proceso(proceso);

		t_paquete *paquete = esperar_respuesta_cpu(socket_cpu_dispatch);
		t_list *datos = deserealizar_paquete(paquete);
		t_pcb *pcb;
		switch(paquete->codigo_operacion) {
			case BLOQUEAR_PROCESO:
				log_info(kernel_logger, "Proceso ejecuto I/O, enviando a cola de bloqueo...");
				pcb = deserializar_pcb(datos, kernel_logger);

				pthread_mutex_lock(&mutex_blocked);
				queue_push(cola_blocked, pcb);
				pthread_mutex_unlock(&mutex_blocked);

				sem_post(&sem_blocked);
				break;
			case FINALIZAR_PROCESO:
				log_info(kernel_logger, "Proceso ejecuto EXIT, enviando a cola de salida...");
				pcb = deserializar_pcb(datos, kernel_logger);

				pthread_mutex_lock(&mutex_exit);
				queue_push(cola_exit, pcb);
				pthread_mutex_unlock(&mutex_exit);

				sem_post(&sem_exit);
				break;
			case PROCESO_DESALOJADO:
				log_info(kernel_logger, "Proceso desalojado por interrupcion, seleccionar siguiente proceso a ejecutar...");
				/*pcb = deserializar_pcb(datos, kernel_logger);
				usleep(20000); // <-- Solo para debug
				log_info(kernel_logger, "Se envia siguiente proceso...");
				enviar_proceso_a_cpu(pcb, socket_cpu_dispatch);
				eliminar_proceso(pcb);*/
				break;
		}
		list_destroy_and_destroy_elements(datos, free);
		eliminar_paquete(paquete);
	}
}

void estado_blocked(void *data) {
	while(1) {
		sem_wait(&sem_blocked);
		pthread_mutex_lock(&mutex_blocked);
		t_pcb *proceso = queue_pop(cola_blocked);
		proceso->estado = BLOCKED;
		proceso->tiempo_inicio_bloqueo = get_tiempo_actual();
		pthread_mutex_unlock(&mutex_blocked);

		//log_info(kernel_logger, "Bloqueando %d ms proceso con ID = %d e inicio = %d...",
				//proceso->tiempo_io, proceso->id, proceso->tiempo_inicio_bloqueo);
		//usleep(proceso->tiempo_io * 1000);
		pthread_t th_io;
		//pthread_t th_suspend;
		pthread_create(&th_io, NULL, (void *)io, proceso);
		//pthread_create(&th_suspend, NULL, (void *)suspend, proceso);
		pthread_detach(th_io);
		//pthread_detach(th_suspend);

		//log_info(kernel_logger, "Finalizo bloqueo...");

		//pthread_mutex_lock(&mutex_ready);
		//queue_push(cola_ready, proceso);
		//pthread_mutex_unlock(&mutex_ready);

		//sem_post(&sem_ready);
	}
}

void io(t_pcb *pcb) {
	pthread_mutex_lock(&mutex_io);
	usleep(pcb->tiempo_io * 1000);
	pthread_mutex_unlock(&mutex_io);

	uint32_t tiempo_bloqueo = tiempo_bloqueado(pcb->tiempo_inicio_bloqueo);
	log_info(kernel_logger, "[IO] -> ID = %d, tiempo io = %d, tiempo bloqueado = %d...",
			pcb->id, pcb->tiempo_io, tiempo_bloqueo);

	if(tiempo_bloqueo >= kernel_config->tiempo_maximo_bloqueado) {
		pcb->estado = SUSPENDED_BLOCKED;
		enviar_proceso_a_memoria(pcb, socket_memoria);
		esperar_respuesta_memoria(socket_memoria);
	}

	if(pcb->estado == BLOCKED) {
		log_info(kernel_logger, "Proceso quedo BLOCKED...");
		pthread_mutex_lock(&mutex_ready);
		queue_push(cola_ready, pcb);
		pthread_mutex_unlock(&mutex_ready);
		sem_post(&sem_ready);
	} else if(pcb->estado == SUSPENDED_BLOCKED) {
		log_info(kernel_logger, "Proceso quedo SUSPENDED-BLOCKED...");
		pthread_mutex_lock(&mutex_ready);
		queue_push(cola_ready, pcb);
		pthread_mutex_unlock(&mutex_ready);
		sem_post(&sem_ready);
	}

}

void suspend(t_pcb *pcb) {
	while(1) {
		pthread_mutex_lock(&mutex_suspend);
		if(pcb->estado == BLOCKED && tiempo_bloqueado(pcb->tiempo_inicio_bloqueo) >= kernel_config->tiempo_maximo_bloqueado) {
			log_info(kernel_logger, "TIME BLOCK = %d...", tiempo_bloqueado(pcb->tiempo_inicio_bloqueo));
			pcb->estado = SUSPENDED_BLOCKED;
			//pthread_mutex_unlock(&mutex_suspend);
			enviar_proceso_a_memoria(pcb, socket_memoria);
			esperar_respuesta_memoria(socket_memoria);
			//pthread_exit(NULL);
		}
		//pthread_mutex_lock(&mutex_suspend);
		if(pcb->tiempo_io == 0) {
			log_info(kernel_logger, "TIME BLOCK = %d...", tiempo_bloqueado(pcb->tiempo_inicio_bloqueo));
			//pthread_mutex_unlock(&mutex_suspend);
			if(pcb->estado == BLOCKED) {
				log_info(kernel_logger, "Proceso quedo BLOCKED...");
				pthread_mutex_lock(&mutex_ready);
				queue_push(cola_ready, pcb);
				pthread_mutex_unlock(&mutex_ready);
				sem_post(&sem_ready);
			} else if(pcb->estado == SUSPENDED_BLOCKED) {
				log_info(kernel_logger, "Proceso quedo SUSPENDED-BLOCKED...");
				pthread_mutex_lock(&mutex_ready);
				queue_push(cola_ready, pcb);
				pthread_mutex_unlock(&mutex_ready);
				sem_post(&sem_ready);
				//pthread_mutex_lock(&mutex_suspended_ready);
				//queue_push(cola_suspended_ready, pcb);
				//pthread_mutex_unlock(&mutex_suspended_ready);
			}
			pthread_mutex_unlock(&mutex_suspend);
			break;
		}
		pthread_mutex_unlock(&mutex_suspend);
		//usleep(10);
	}
	pthread_exit(NULL);
}

uint32_t tiempo_bloqueado(uint32_t tiempo) {
	return get_tiempo_actual() - tiempo;
}

void enviar_proceso_a_cpu(t_pcb *pcb, int socket_cpu_dispatch) {
	t_paquete *paquete = serializar_pcb(pcb, PCB);
	enviar_paquete(paquete, socket_cpu_dispatch);
	eliminar_paquete(paquete);
}

t_paquete *esperar_respuesta_cpu(int socket_cpu_dispatch) {
	return recibir_paquete(socket_cpu_dispatch);
}


void enviar_interrupcion_a_cpu(int socket_fd){
	log_info(kernel_logger, "Enviando interrupcion de desalojo...");
	t_paquete *paquete = crear_paquete(DESALOJAR_PROCESO, buffer_vacio());
	uint32_t nada = 0;
	agregar_a_paquete(paquete, &nada, sizeof(uint32_t));
	enviar_paquete(paquete, socket_fd);
	eliminar_paquete(paquete);
}








