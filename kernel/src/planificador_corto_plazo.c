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
	enviar_proceso_a_cpu(proceso, socket_cpu_dispatch);
	eliminar_proceso(proceso);

	t_protocolo protocolo;
	do {
		t_paquete *paquete = esperar_respuesta_cpu(socket_cpu_dispatch);
		protocolo = paquete->codigo_operacion;
		analizar_datos(paquete);
		eliminar_paquete(paquete);
	} while (protocolo != FINALIZAR_PROCESO);
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
	t_list *datos = deserealizar_paquete(paquete);
	t_pcb *pcb;
	switch(paquete->codigo_operacion) {
		case BLOQUEAR_PROCESO:
			log_info(kernel_logger, "Proceso ejecuto IO, enviando a cola de bloqueo...");
			uint32_t *tiempo_bloqueo = (uint32_t *)list_remove(datos, list_size(datos) - 1);
			pcb = deserializar_pcb(datos, kernel_logger);
			log_info(kernel_logger, "Tiempo bloqueo = %d", *tiempo_bloqueo);
			usleep(*tiempo_bloqueo * 1000);
			log_info(kernel_logger, "Finalizo bloqueo, enviando proceso a CPU...");
			enviar_proceso_a_cpu(pcb, socket_cpu_dispatch);
			eliminar_proceso(pcb);
			free(tiempo_bloqueo);
			break;
		case FINALIZAR_PROCESO:
			log_info(kernel_logger, "Proceso finalizado, liberando recursos...");
			pcb = deserializar_pcb(datos, kernel_logger);
			eliminar_proceso(pcb);
			break;
		case PROCESO_DESALOJADO:
			log_info(kernel_logger, "Proceso desalojado por interrupcion, seleccionar siguiente proceso a ejecutar...");
			break;
	}
	list_destroy_and_destroy_elements(datos, free);
}

void enviar_proceso_a_cpu(t_pcb *pcb, int socket_cpu_dispatch) {
	t_paquete *paquete = serializar_pcb(pcb, PCB);
	enviar_paquete(paquete, socket_cpu_dispatch);
	eliminar_paquete(paquete);
}

t_paquete *esperar_respuesta_cpu(int socket_cpu_dispatch) {
	return recibir_paquete(socket_cpu_dispatch);
}

