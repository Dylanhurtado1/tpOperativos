#include "gestionar_consola.h"


void procesar_datos_consola(t_cliente *datos_cliente) {
	t_paquete *paquete = datos_cliente->paquete;

	switch (paquete->codigo_operacion) {
		case DATOS_CONSOLA:
			log_info(kernel_logger, "Procesando datos de consola");
			t_list *datos = deserealizar_paquete(paquete);
			t_list *instrucciones = deserializar_instrucciones(datos, kernel_logger);
			uint32_t tamanio = deserialzar_tamanio_consola(datos);
			print_datos_consola(kernel_logger, instrucciones, tamanio);

			t_pcb *proceso = crear_estructura_pcb(instrucciones, tamanio);
			agregar_proceso_a_new(proceso, datos_cliente->socket);

			list_destroy_and_destroy_elements(datos, free);
			break;
		default:
			log_error(kernel_logger,"Protocolo invalido.");
			break;
	}
}
