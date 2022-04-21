#include "gestionar_consola.h"

extern t_log *kernel_logger;

void procesar_datos_consola(t_cliente *datos_cliente) {
	t_list *datos;
	t_list *lista_instrucciones;
	uint32_t tamanio_consola;
	t_paquete *paquete = datos_cliente->paquete;
	switch (paquete->codigo_operacion) {
		case DATOS_CONSOLA:
			log_info(kernel_logger, "Procesando DATOS_CONSOLA");
			datos = deserealizar_paquete(paquete);
			lista_instrucciones = deserealizar_instrucciones(datos);
			tamanio_consola = obtener_tamanio_consola(datos);
			log_info(kernel_logger,"Cantidad de instrucciones: %d", list_size(lista_instrucciones));
			log_info(kernel_logger,"Tamanio de consola: %d", tamanio_consola);
			list_destroy_and_destroy_elements(lista_instrucciones, free);
			list_destroy_and_destroy_elements(datos, free);
			enviar_respuesta_a_consola(datos_cliente->socket, FINALIZAR_CONSOLA_OK);
			break;
		default:
			log_error(kernel_logger,"Protocolo invalido.");
			break;
	}
}

void enviar_respuesta_a_consola(int socket_fd, t_protocolo protocolo) {
	enviar_datos(socket_fd, &protocolo, sizeof(t_protocolo));
}

t_list *deserealizar_instrucciones(t_list *datos) {
	t_list *lista_instrucciones = list_create();

	for(int i = 0; i < list_size(datos) - 1; i += 3) {
		t_instruccion *instruccion = malloc(sizeof(t_instruccion));
		instruccion->identificador = *(t_identificador *)list_get(datos, i);
		instruccion->primer_operando = *(uint32_t *)list_get(datos, i + 1);
		instruccion->segundo_operando = *(uint32_t *)list_get(datos, i + 2);
		list_add(lista_instrucciones, instruccion);
	}

	return lista_instrucciones;
}

uint32_t obtener_tamanio_consola(t_list *datos) {
	uint32_t tamanio = *(uint32_t *)list_get(datos, list_size(datos) - 1);

	return tamanio;
}

