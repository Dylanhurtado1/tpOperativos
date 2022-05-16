#include "planificador.h"

//se encarga de los estados suspendidos

void eliminar_proceso_cola_blocked(t_pcb *proceso);

t_queue *cola_blocked;
t_queue *cola_suspended_blocked;
t_queue *cola_suspended_ready;
extern t_kernel_config *kernel_config;



void iniciar_cola_blocked(){
	cola_blocked = queue_create();
}


void iniciar_cola_suspended_blocked() {
	cola_suspended_blocked = queue_create();
}


void iniciar_cola_suspended_ready() {
	cola_suspended_ready = queue_create();
}


void agregar_proceso_a_suspended_blocked(t_pcb *proceso) {
	queue_push(cola_suspended_blocked, proceso);
	//enviar_datos_a_memoria(proceso, socket_memoria)
}


void agregar_proceso_a_suspended_ready(t_pcb *proceso) {
	queue_push(cola_suspended_ready, proceso);
}


void eliminar_cola_blocked() {
	queue_destroy_and_destroy_elements(cola_blocked, (void *)eliminar_proceso_cola_blocked);
}


void eliminar_proceso_cola_blocked(t_pcb *proceso) {
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}


uint32_t tiempo_maximo_de_bloqueo(){
	return kernel_config->tiempo_maximo_bloqueado;
}


/*
void suspenderProceso(t_pcb *proceso){
	if(??? > tiempo_maximo_de_bloqueo()){ 			 Necesito el tiempo que esta bloqueado, se podra agregar un atributo a la PCB y que vaya sumando
		agregar_proceso_a_suspended_blocked(proceso);	desde el momento que ingresa a la cola de blocked? (habla de usleep en el tp pero mepa que no me sirve)
	}

}

*/



