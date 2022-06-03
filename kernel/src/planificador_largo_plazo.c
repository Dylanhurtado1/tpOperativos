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
	proceso->pcb = crear_estructura_pcb(consola);
	proceso->estado = JOB;
	proceso->tiempo_io = 0;
	proceso->tiempo_inicio_bloqueo = 0;
	proceso->tiempo_cpu = 0;
	return proceso;
}

t_pcb *crear_estructura_pcb(t_consola *consola) {
	t_pcb *pcb = malloc(sizeof(t_pcb));

	pthread_mutex_lock(&mutex_generador_id);
	pcb->id = generador_de_id;
	generador_de_id++;
	pthread_mutex_unlock(&mutex_generador_id);
	pcb->tamanio_proceso = consola->tamanio;
	pcb->instrucciones = list_duplicate(consola->instrucciones);
	pcb->program_counter = 0;
	pcb->estimacion_rafaga = kernel_config->estimacion_inicial;

	return pcb;
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
			proceso->pcb->tabla_paginas = obtener_entrada_tabla_de_pagina(socket_memoria);
			log_info(kernel_logger, "PID[%d] ingresa a READY desde NEW", proceso->pcb->id);
		}

		pthread_mutex_lock(&mutex_ready);
		list_add(cola_ready, proceso);
		pthread_mutex_unlock(&mutex_ready);

		//procesos_admitidos_en_ready++;
		sem_post(&sem_ready);
	}
}

uint32_t obtener_entrada_tabla_de_pagina(int socket_fd) {
	uint32_t numero;
	uint32_t fake_data = 0;
	t_paquete *paquete = crear_paquete(AGREGAR_PROCESO_A_MEMORIA, buffer_vacio());
	agregar_a_paquete(paquete, &fake_data, sizeof(uint32_t)); // TODO: problemas con paquete sin datos
	enviar_paquete(paquete, socket_fd);
	eliminar_paquete(paquete);

	recibir_datos(socket_fd, &numero, sizeof(uint32_t));

	return numero;
}

void estado_exit(void *dato) {
	while(1) {
		sem_wait(&sem_exit);
		t_proceso *proceso = queue_pop(cola_exit);
		log_info(kernel_logger, "PID[%d] ingresa a EXIT", proceso->pcb->id);

		enviar_proceso_a_memoria(proceso, socket_memoria, ELIMINAR_MEMORIA_PCB);
		t_protocolo protocolo = esperar_respuesta_memoria(socket_memoria);
		if(protocolo != PCB_ELIMINADO) {
			log_error(kernel_logger, "No se pudo eliminar memoria de PID[%d]", proceso->pcb->id);
		}

		enviar_respuesta_a_consola(proceso->socket, FINALIZAR_CONSOLA_OK);

		eliminar_proceso(proceso);
		sem_post(&sem_grado_multiprogramacion);
	}
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
	list_destroy_and_destroy_elements(proceso->pcb->instrucciones, free);
	free(proceso->pcb);
	free(proceso);
}



