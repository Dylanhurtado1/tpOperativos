#include "planificador.h"


void iniciar_planificador_largo_plazo() {
	generador_de_id = 0;
	//procesos_admitidos_en_ready = 0;
	pthread_mutex_init(&mutex_new, NULL);
	pthread_mutex_init(&mutex_exit, NULL);
	pthread_mutex_init(&mutex_generador_id, NULL);
	sem_init(&sem_admitir, 0, 0);
	sem_init(&sem_exit, 0, 0);
	sem_init(&sem_grado_multiprogramacion, 0, kernel_config->grado_multiprogramacion);
	cola_new = queue_create();
	cola_exit = queue_create();
	pthread_create(&thread_exit, NULL, (void *)estado_exit, NULL);
	pthread_create(&thread_admitir, NULL, (void *)transicion_admitir, NULL);
	pthread_detach(thread_exit);
	pthread_detach(thread_admitir);
}

t_proceso *crear_proceso(t_consola *consola, int socket_consola) {
	t_proceso *proceso = malloc(sizeof(t_proceso));

	proceso->socket = socket_consola;
	proceso->estado = JOB;
	proceso->tiempo_io = 0;
	proceso->tiempo_inicio_bloqueo = 0;
	proceso->tiempo_cpu = 0;

	pthread_mutex_lock(&mutex_generador_id);
	proceso->pcb = crear_pcb(generador_de_id, consola->tamanio, consola->instrucciones, 0, kernel_config->estimacion_inicial);
	generador_de_id++;
	pthread_mutex_unlock(&mutex_generador_id);

	return proceso;
}

void agregar_proceso_a_new(t_proceso *proceso) {
	pthread_mutex_lock(&mutex_new);
	queue_push(cola_new, proceso);
	pthread_mutex_unlock(&mutex_new);
	log_info(kernel_logger, "PID[%d] ingresa a NEW...", proceso->pcb->id);

	sem_post(&sem_admitir);
}

void transicion_admitir(void *data) {
	while(1) {
		sem_wait(&sem_admitir);
		sem_wait(&sem_grado_multiprogramacion);
		t_proceso *proceso;

		pthread_mutex_lock(&mutex_suspended_ready);
		if(!queue_is_empty(cola_suspended_ready)) {
			proceso = queue_pop(cola_suspended_ready);
			pthread_mutex_unlock(&mutex_suspended_ready);
			log_info(kernel_logger, "PID[%d] ingresa a READY desde SUSPENDED-READY", proceso->pcb->id);
		} else {
			pthread_mutex_unlock(&mutex_suspended_ready);
			pthread_mutex_lock(&mutex_new);
			proceso = queue_pop(cola_new);
			pthread_mutex_unlock(&mutex_new);
			proceso->pcb->tabla_paginas = obtener_tabla_de_pagina(socket_memoria, proceso->pcb);
			log_info(kernel_logger, "PID[%d] ingresa a READY desde NEW", proceso->pcb->id);
		}

		pthread_mutex_lock(&mutex_ready);
		list_add(cola_ready, proceso);
		pthread_mutex_unlock(&mutex_ready);

		//procesos_admitidos_en_ready++;
		sem_post(&sem_ready);
	}
}

void estado_exit(void *dato) {
	while(1) {
		sem_wait(&sem_exit);
		t_proceso *proceso = queue_pop(cola_exit);
		log_info(kernel_logger, "PID[%d] ingresa a EXIT", proceso->pcb->id);

		enviar_proceso_a_memoria(proceso, socket_memoria, FINALIZACION_DE_PROCESO);
		t_protocolo protocolo = esperar_respuesta_memoria(socket_memoria);
		if(protocolo != PROCESO_FINALIZADO) {
			log_error(kernel_logger, "No se pudo eliminar memoria de PID[%d]", proceso->pcb->id);
		}

		enviar_respuesta_a_consola(proceso->socket, FINALIZAR_CONSOLA);

		eliminar_proceso(proceso);
		sem_post(&sem_grado_multiprogramacion);
	}
}

uint32_t obtener_tabla_de_pagina(int socket_memoria, t_pcb *pcb) {
	uint32_t tabla_de_paginas;
	t_paquete *paquete = serializar_pcb(pcb, INICIALIZACION_DE_PROCESO);
	enviar_paquete(paquete, socket_memoria);

	recibir_datos(socket_memoria, &tabla_de_paginas, sizeof(uint32_t));
	eliminar_paquete(paquete);

	return tabla_de_paginas;
}

void enviar_proceso_a_memoria(t_proceso *proceso, int socket_memoria, t_protocolo protocolo) {
	t_paquete *paquete = serializar_pcb(proceso->pcb, protocolo);
	enviar_paquete(paquete, socket_memoria);
	eliminar_paquete(paquete);
}

t_protocolo esperar_respuesta_memoria(int socket_memoria) {
	t_protocolo protocolo;
	recibir_datos(socket_memoria, &protocolo, sizeof(t_protocolo));
	return protocolo;
}

void enviar_respuesta_a_consola(int socket_fd, t_protocolo protocolo) {
	enviar_datos(socket_fd, &protocolo, sizeof(t_protocolo));
}

void eliminar_proceso(t_proceso *proceso) {
	eliminar_pcb(proceso->pcb);
	free(proceso);
}

