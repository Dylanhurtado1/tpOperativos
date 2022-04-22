#include "planificador.h"

void eliminar_proceso_cola_ready(t_pcb *proceso);

t_queue *cola_ready;

void agregar_proceso_a_ready(t_pcb *proceso) {
	queue_push(cola_ready, proceso);
}

void iniciar_cola_ready() {
	cola_ready = queue_create();
}

void eliminar_cola_ready() {
	queue_destroy_and_destroy_elements(cola_ready, (void *)eliminar_proceso_cola_ready);
}

void eliminar_proceso_cola_ready(t_pcb *proceso) {
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}
