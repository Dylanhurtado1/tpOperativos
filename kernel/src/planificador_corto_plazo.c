#include "planificador.h"

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
		pthread_mutex_unlock(&mutex_blocked);

		log_info(kernel_logger, "Bloqueando %d ms proceso con ID = %d...", proceso->tiempo_io, proceso->id);
		usleep(proceso->tiempo_io * 1000);
		log_info(kernel_logger, "Finalizo bloqueo...");

		pthread_mutex_lock(&mutex_ready);
		queue_push(cola_ready, proceso);
		pthread_mutex_unlock(&mutex_ready);

		sem_post(&sem_ready);
	}
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








