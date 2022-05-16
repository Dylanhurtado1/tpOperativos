#include "planificador.h"

void generar_pid(uint32_t id, int socket_fd);
void eliminar_pid(t_pid *pid);

t_list *pids;

void iniciar_planificador_largo_plazo() {
	generador_de_id = 0;
	procesos_admitidos_en_ready = 0;
	pthread_mutex_init(&mutex_new, NULL);
	pthread_mutex_init(&mutex_exit, NULL);
	pthread_mutex_init(&mutex_generador_id, NULL);
	sem_init(&sem_exit, 0, 0);
	sem_init(&sem_grado_multiprogramacion, 0, kernel_config->grado_multiprogramacion);
	cola_new = queue_create();
	cola_exit = queue_create();
	pids = list_create();
	pthread_create(&thread_exit, NULL, (void *)estado_exit, NULL);
}

t_pcb *crear_estructura_pcb(t_list *instrucciones, uint32_t tam_proceso) {
	t_pcb *pcb = malloc(sizeof(t_pcb));

	pthread_mutex_lock(&mutex_generador_id);
	pcb->id = generador_de_id;
	generador_de_id++;
	pthread_mutex_unlock(&mutex_generador_id);
	pcb->tam_proceso = tam_proceso;
	pcb->instrucciones = instrucciones;
	pcb->program_counter = 0;
	pcb->estimacion_rafaga = kernel_config->estimacion_inicial;

	return pcb;
}

void agregar_proceso_a_new(t_pcb *proceso, int socket_fd) {
	generar_pid(proceso->id, socket_fd);

	pthread_mutex_lock(&mutex_new);
	queue_push(cola_new, proceso);
	pthread_mutex_unlock(&mutex_new);
	log_info(kernel_logger, "Se agregó PCB a cola NEW");

	sem_post(&sem_ready);
}

void generar_pid(uint32_t id, int socket_fd) {
	t_pid *pid = malloc(sizeof(t_pid));
	pid->socket = socket_fd;
	pid->id = id;
	list_add(pids, pid);
}





bool es_posible_admitir_proceso() {
	return procesos_admitidos_en_ready < kernel_config->grado_multiprogramacion;
}

void admitir_proceso() {
	pthread_mutex_lock(&mutex_new);
	t_pcb *proceso = (t_pcb *)queue_pop(cola_new);
	pthread_mutex_unlock(&mutex_new);
	proceso->tabla_paginas = obtener_numero_tabla_de_pagina(socket_memoria);

	pthread_mutex_lock(&mutex_ready);
	queue_push(cola_ready, proceso);
	pthread_mutex_unlock(&mutex_ready);
	procesos_admitidos_en_ready++;
}


void eliminar_cola_new() {
	queue_destroy_and_destroy_elements(cola_new, (void *)eliminar_proceso_cola_new);
}

uint32_t obtener_numero_tabla_de_pagina(int socket_fd) {
	uint32_t numero;
	uint32_t fake_data = 0;
	t_paquete *paquete = crear_paquete(AGREGAR_PROCESO_A_MEMORIA, buffer_vacio());
	agregar_a_paquete(paquete, &fake_data, sizeof(uint32_t)); // TODO: problemas con paquete sin datos
	enviar_paquete(paquete, socket_fd);
	eliminar_paquete(paquete);

	recibir_datos(socket_fd, &numero, sizeof(uint32_t));
	log_info(kernel_logger, "Numero de tabla de paginas: %d", numero);

	return numero;
}




void estado_exit(void *dato) {
	while(1) {
		sem_wait(&sem_exit);
		t_pcb *pcb = (t_pcb *)queue_pop(cola_exit);
		// TODO: enviar info a memoria

		bool buscar_id(t_pid *pid) {
			return pid->id == pcb->id;
		}
		t_pid *pid = list_remove_by_condition(pids, (void *)buscar_id);
		enviar_respuesta_a_consola(pid->socket, FINALIZAR_CONSOLA_OK);

		eliminar_proceso_cola_new(pcb);
		eliminar_pid(pid);
		sem_post(&sem_grado_multiprogramacion);
	}
}

void enviar_respuesta_a_consola(int socket_fd, t_protocolo protocolo) {
	enviar_datos(socket_fd, &protocolo, sizeof(t_protocolo));
}

void eliminar_proceso_cola_new(t_pcb *proceso) {
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}

void eliminar_pid(t_pid *pid) {
	free(pid);
}



