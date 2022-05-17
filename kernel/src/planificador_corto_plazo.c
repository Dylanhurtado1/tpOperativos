#include "planificador.h"

void enviar_interrupcion_a_cpu(int socket_fd);
void enviar_proceso_a_memoria(t_pcb *pcb, int socket_memoria);
t_paquete *esperar_respuesta_memoria(int socket_memoria);

void iniciar_planificador_corto_plazo() {
	tiempo_bloqueo = NULL;
	pthread_mutex_init(&mutex_ready, NULL);
	pthread_mutex_init(&mutex_blocked, NULL);
	pthread_mutex_init(&mutex_exec, NULL);
	sem_init(&sem_ready, 0, 0);
	sem_init(&sem_exec, 0, 0);
	sem_init(&sem_blocked, 0, 0);
	cola_ready = queue_create();
	cola_exec = queue_create();
	cola_blocked = queue_create();
	pthread_create(&thread_ready, NULL, (void *)estado_ready, NULL);
	pthread_create(&thread_exec, NULL, (void *)estado_exec, NULL);
	pthread_create(&thread_blocked, NULL, (void *)estado_blocked, NULL);
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
				log_info(kernel_logger, "Proceso ejecuto IO, enviando a cola de bloqueo...");
				tiempo_bloqueo = (uint32_t *)list_remove(datos, list_size(datos) - 1);
				log_info(kernel_logger, "Tiempo bloqueo = %d ms", *tiempo_bloqueo);
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
		log_info(kernel_logger, "Bloqueando proceso con ID = %d...", proceso->id);
		usleep(*tiempo_bloqueo * 1000);
		free(tiempo_bloqueo);
		log_info(kernel_logger, "Finalizo bloqueo...");

		pthread_mutex_lock(&mutex_ready);
		queue_push(cola_ready, proceso);
		pthread_mutex_unlock(&mutex_ready);
		sem_post(&sem_ready);
	}
}



void agregar_proceso_a_ready(t_pcb *proceso) {
	queue_push(cola_ready, proceso);
	// TODO: agregar condicion para saber si es SRT (la interrupcion solo se envia en SRT)
	enviar_interrupcion_a_cpu(socket_cpu_interrupt);
}

void ejecutar_proceso() {
	/*if(hay_proceso_en_ejecucion()) {
		log_info(kernel_logger, "Hay un proceso en ejecucion");
		return;
	}*/
	t_pcb *proceso = queue_pop(cola_ready);
	enviar_proceso_a_cpu(proceso, socket_cpu_dispatch);//todos los procesos son enviados por dispatch
	//eliminar_proceso(proceso);

	t_protocolo protocolo;
	do {
		t_paquete *paquete = esperar_respuesta_cpu(socket_cpu_dispatch);
		protocolo = paquete->codigo_operacion;
		analizar_datos(paquete);
		eliminar_paquete(paquete);
	} while (protocolo != FINALIZAR_PROCESO);
	procesos_admitidos_en_ready--;
}

/*bool hay_proceso_en_ejecucion() {
	return exec;
}*/

void iniciar_cola_ready() {
	cola_ready = queue_create();
}


void iniciar_cola_exec(){
	cola_exec = queue_create();
}

void analizar_datos(t_paquete *paquete) {
	t_list *datos = deserealizar_paquete(paquete);
	t_pcb *pcb;
	switch(paquete->codigo_operacion) {
		case BLOQUEAR_PROCESO:
			log_info(kernel_logger, "Proceso ejecuto IO, enviando a cola de bloqueo...");
			uint32_t *tiempo_bloqueo = (uint32_t *)list_remove(datos, list_size(datos) - 1);
			pcb = deserializar_pcb(datos, kernel_logger);
			//agregar_proceso_a_blocked(pcb);//se agrega a la cola de bloqueados
			log_info(kernel_logger, "Tiempo bloqueo = %d", *tiempo_bloqueo);
			usleep(*tiempo_bloqueo * 1000);
			log_info(kernel_logger, "Finalizo bloqueo, enviando proceso a CPU...");
			enviar_proceso_a_cpu(pcb, socket_cpu_dispatch);
			eliminar_proceso(pcb);
			free(tiempo_bloqueo);
			break;
		case FINALIZAR_PROCESO:
			log_info(kernel_logger, "Proceso finalizado, liberando recursos a memoria...");//todavia no implementado
			pcb = deserializar_pcb(datos, kernel_logger);
			enviar_proceso_a_memoria(pcb, socket_memoria); //falta la parte de memoria :(
			esperar_respuesta_memoria(socket_memoria);
			log_info(kernel_logger,"Aviso a consola que finaliza proceso :)");
			//mandar un mensaje a memoria
			//espera respuesta de memoria
			//envia mensaje a consola
			eliminar_proceso(pcb);
			break;
		case PROCESO_DESALOJADO:
			log_info(kernel_logger, "Proceso desalojado por interrupcion, seleccionar siguiente proceso a ejecutar...");
			pcb = deserializar_pcb(datos, kernel_logger);
			usleep(20000); // <-- Solo para debug
			log_info(kernel_logger, "Se envia siguiente proceso...");
			enviar_proceso_a_cpu(pcb, socket_cpu_dispatch);
			eliminar_proceso(pcb);
			break;
	}
	list_destroy_and_destroy_elements(datos, free);
}

void enviar_proceso_a_cpu(t_pcb *pcb, int socket_cpu_dispatch) {
	t_paquete *paquete = serializar_pcb(pcb, PCB);
	enviar_paquete(paquete, socket_cpu_dispatch);
	eliminar_paquete(paquete);
}

void enviar_proceso_a_memoria(t_pcb *pcb, int socket_memoria) {
	t_paquete *paquete = serializar_pcb(pcb, PCB);
	enviar_paquete(paquete, socket_memoria);
	eliminar_paquete(paquete);
}


t_paquete *esperar_respuesta_cpu(int socket_cpu_dispatch) {
	return recibir_paquete(socket_cpu_dispatch);
}

t_paquete *esperar_respuesta_memoria(int socket_memoria) {//nose si seria con dispath
	return recibir_paquete(socket_memoria);
}

bool hay_procesos_en_ready(){
	int cantidadReady = queue_size(cola_ready);
	return cantidadReady > 0;
}

void agregar_proceso_a_blocked(t_pcb *proceso) {
	queue_push(cola_blocked, proceso);
}

void enviar_interrupcion_a_cpu(int socket_fd){
	log_info(kernel_logger, "Enviando interrupcion de desalojo");
	t_paquete *paquete = crear_paquete(DESALOJAR_PROCESO, buffer_vacio());
	uint32_t nada = 0;
	agregar_a_paquete(paquete, &nada, sizeof(uint32_t));
	enviar_paquete(paquete, socket_fd);
	eliminar_paquete(paquete);
}








