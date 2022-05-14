#include "planificador.h"

t_pcb *crear_estructura_pcb(t_list *instrucciones, uint32_t tam_proceso);
void eliminar_proceso_cola_new(t_pcb *proceso);
uint32_t obtener_numero_tabla_de_pagina(int socket_fd);

uint32_t generador_de_id = 0;
uint32_t procesos_admitidos_en_ready = 0;
t_queue *cola_new;
t_queue *cola_fin;

extern t_log *kernel_logger;
extern t_kernel_config *kernel_config;
extern int socket_memoria;


void iniciar_cola_new() {
	cola_new = queue_create();
}

void iniciar_cola_fin(){
	cola_fin = queue_create();
}


void agregar_proceso_a_new(t_list *instrucciones, uint32_t tam_proceso) {
	t_pcb *proceso = crear_estructura_pcb(instrucciones, tam_proceso);

	pthread_mutex_lock(&planificador_mutex_new);
	queue_push(cola_new, proceso);
	pthread_mutex_unlock(&planificador_mutex_new);
}

t_pcb *crear_estructura_pcb(t_list *instrucciones, uint32_t tam_proceso) {
	t_pcb *pcb = malloc(sizeof(t_pcb));

	pcb->id = generador_de_id;
	pcb->tam_proceso = tam_proceso;
	pcb->instrucciones = instrucciones;
	pcb->program_counter = 0;
	pcb->estimacion_rafaga = kernel_config->estimacion_inicial;

	generador_de_id++;
	return pcb;
}

bool es_posible_admitir_proceso() {
	return procesos_admitidos_en_ready < kernel_config->grado_multiprogramacion;
}

void admitir_proceso() {
	uint32_t numero = obtener_numero_tabla_de_pagina(socket_memoria);
	t_pcb *proceso = (t_pcb *)queue_pop(cola_new);
	proceso->tabla_paginas = numero;
	pthread_mutex_lock(&planificador_mutex_ready);//semaforos mutex cada vez que metemos proceso
	agregar_proceso_a_ready(proceso);
	pthread_mutex_unlock(&planificador_mutex_ready);
	procesos_admitidos_en_ready++;
}


void eliminar_cola_new() {
	queue_destroy_and_destroy_elements(cola_new, (void *)eliminar_proceso_cola_new);
}

void eliminar_proceso_cola_new(t_pcb *proceso) {
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
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








