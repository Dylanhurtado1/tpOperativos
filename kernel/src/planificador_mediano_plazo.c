#include "planificador.h"

//se encarga de los estados suspendidos

t_queue *cola_suspended_blocked;
t_queue *cola_suspended_ready;

void agregar_proceso_a_suspended_blocked(t_pcb *proceso) {
	queue_push(cola_suspended_blocked, proceso);
}

void iniciar_cola_suspended_blocked() {
	cola_suspended_blocked = queue_create();
}


