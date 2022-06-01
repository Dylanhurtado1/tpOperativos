#include "peticiones.h"


void peticiones_dispatch(int *socket_dispatch) {
	t_pcb *pcb;

	log_info(cpu_logger, "CPU escuchando puerto dispatch");
	int socket_kernel = esperar_cliente(*socket_dispatch);
	while (true) {
		t_paquete *paquete = recibir_paquete(socket_kernel);
		switch (paquete->codigo_operacion) {
			case PCB:
				pcb = deserializar_pcb(paquete);
				log_info(cpu_logger,"PCB[%d] recibido, ejecutando instrucciones", pcb->id);

				ejecutar_ciclo_de_instruccion(pcb, socket_kernel);

				eliminar_pcb(pcb);
				break;
			default:
				log_error(cpu_logger, "Protocolo invalido.");
				break;
		}
		eliminar_paquete(paquete);
	}
}

void peticiones_interrupt(int *socket_interrupt) {
	log_info(cpu_logger, "CPU escuchando puerto interrupt");
	int socket_kernel = esperar_cliente(*socket_interrupt);
	while (true) {
		t_paquete *paquete = recibir_paquete(socket_kernel);
		switch (paquete->codigo_operacion) {
			case DESALOJAR_PROCESO:
				log_info(cpu_logger, "Interrupcion recibida, se desalojara proceso...");
				pthread_mutex_lock(&mutex_interrupt);
				interrupcion_desalojo = true;
				pthread_mutex_unlock(&mutex_interrupt);
				break;
			default:
				log_error(cpu_logger, "Protocolo invalido.");
				break;
		}
		eliminar_paquete(paquete);
	}
}

void eliminar_pcb(t_pcb *pcb) {
	list_destroy_and_destroy_elements(pcb->instrucciones, free);
	free(pcb);
}

