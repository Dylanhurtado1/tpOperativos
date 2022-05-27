#include "gestionar_consola.h"


void procesar_datos_consola(t_cliente *datos_cliente) {
	t_paquete *paquete = datos_cliente->paquete;

	switch (paquete->codigo_operacion) {
		case DATOS_CONSOLA:
			log_info(kernel_logger, "Procesando datos de consola");
			t_consola *consola = deserializar_consola(paquete);
			print_datos_consola(kernel_logger, consola);

			t_pcb *proceso = crear_estructura_pcb(consola);
			agregar_proceso_a_new(proceso, datos_cliente->socket);

			eliminar_consola(consola);
			break;
		default:
			log_error(kernel_logger,"Protocolo invalido.");
			break;
	}
}

void eliminar_consola(t_consola *consola) {
	list_destroy_and_destroy_elements(consola->instrucciones, free);
	free(consola);
}
