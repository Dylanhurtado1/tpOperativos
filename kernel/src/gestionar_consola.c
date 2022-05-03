#include "gestionar_consola.h"

extern t_log *kernel_logger;

void procesar_datos_consola(t_cliente *datos_cliente) {
	t_list *datos;
	t_list *lista_instrucciones;
	uint32_t tamanio_consola;
	t_paquete *paquete = datos_cliente->paquete;
	switch (paquete->codigo_operacion) {
		case DATOS_CONSOLA:
			log_info(kernel_logger, "Procesando datos de consola");
			datos = deserealizar_paquete(paquete);
			lista_instrucciones = deserializar_instrucciones(datos, kernel_logger);
			tamanio_consola = obtener_tamanio_consola(datos);
			log_info(kernel_logger,"Cantidad de instrucciones recibidas: %d", list_size(lista_instrucciones));
			log_info(kernel_logger,"Tamanio de consola: %d", tamanio_consola);

			agregar_proceso_a_new(lista_instrucciones, tamanio_consola);
			if(es_posible_admitir_proceso()) {
				admitir_proceso();
			}
			ejecutar_proceso();

			enviar_respuesta_a_consola(datos_cliente->socket, FINALIZAR_CONSOLA_OK);
			list_destroy_and_destroy_elements(datos, free);
			break;
		default:
			log_error(kernel_logger,"Protocolo invalido.");
			break;
	}
}

void enviar_respuesta_a_consola(int socket_fd, t_protocolo protocolo) {
	enviar_datos(socket_fd, &protocolo, sizeof(t_protocolo));
}

uint32_t obtener_tamanio_consola(t_list *datos) {
	uint32_t tamanio = *(uint32_t *)list_get(datos, list_size(datos) - 1);

	return tamanio;
}

