/*
 * gestionar_consola.c
 *
 *  Created on: 20 abr. 2022
 *      Author: utnso
 */

#include "gestionar_consola.h"

extern t_log *kernel_logger;

void procesar_operacion(t_cliente *datos_cliente) {
	t_paquete *paquete = datos_cliente->paquete;
	switch (paquete->codigo_operacion) {
		case DATOS_CONSOLA:
			// TODO: implementar la logica cuando recibe este protocolo
			log_info(kernel_logger, "Procesando DATOS_CONSOLA");
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
