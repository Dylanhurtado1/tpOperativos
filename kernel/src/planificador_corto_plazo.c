#include "planificador.h"

void eliminar_proceso_cola_ready(t_pcb *proceso);
void eliminar_proceso(t_pcb *proceso);

t_queue *cola_ready;
bool exec = false;

extern int socket_cpu_dispatch;
extern t_log *kernel_logger;
extern uint32_t procesos_admitidos;

void agregar_proceso_a_ready(t_pcb *proceso) {
	queue_push(cola_ready, proceso);
}

void ejecutar_proceso() {
	if(hay_proceso_en_ejecucion()) {
		log_info(kernel_logger, "Hay un proceso en ejecucion");
		return;
	}
	t_pcb *proceso = queue_pop(cola_ready);
	t_paquete *paquete = serializar_pcb(proceso, PCB);
	enviar_paquete(paquete, socket_cpu_dispatch);
	eliminar_paquete(paquete);

	paquete = recibir_paquete(socket_cpu_dispatch);
	analizar_datos(paquete);
	eliminar_paquete(paquete);

	eliminar_proceso(proceso);
	procesos_admitidos--;
}

bool hay_proceso_en_ejecucion() {
	return exec;
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

void eliminar_proceso(t_pcb *proceso) {
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}

void analizar_datos(t_paquete *paquete) {
	switch(paquete->codigo_operacion) {
		case BLOQUEAR_PROCESO:
			log_info(kernel_logger, "Proceso ejecuto IO, enviando a cola de bloqueo...");
			break;
		case FINALIZAR_PROCESO:
			log_info(kernel_logger, "Proceso finalizado, liberando recursos...");
			break;
		case PROCESO_DESALOJADO:
			log_info(kernel_logger, "Proceso desalojado por interrupcion, seleccionar siguiente proceso a ejecutar...");
			break;
	}
}

