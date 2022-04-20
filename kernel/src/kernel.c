/*
 ============================================================================
 Name        : kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "kernel.h"

#define PATH_KERNEL_CONFIG "/home/utnso/tp-2022-1c-lo-importante-es-aprobar/kernel/kernel.config"

void procesar_operacion(t_cliente *datos_cliente);
void enviar_respuesta_a_consola(int socket_fd, t_protocolo protocolo);

t_log *logger;

int main(void) {
	logger = log_create("kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
	t_kernel_config *config = kernel_leer_configuracion(PATH_KERNEL_CONFIG);

	int socker_kernel = crear_servidor(config->ip_kernel, config->puerto_escucha);
	if(socker_kernel == INIT_SERVER_ERROR) {
		log_error(logger, "No se pudo iniciar el servidor Kernel");
		return EXIT_FAILURE;
	}

	if(atender_clientes(socker_kernel, procesar_operacion) == WAIT_CLIENT_ERROR) {
		log_error(logger, "Error al escuchar clientes... Finalizando servidor");
	}

	log_destroy(logger);
	kernel_eliminar_configuracion(config);
	cerrar_conexion(socker_kernel);

	return EXIT_SUCCESS;
}

void procesar_operacion(t_cliente *datos_cliente) {
	t_paquete *paquete = datos_cliente->paquete;
	switch (paquete->codigo_operacion) {
		case DATOS_CONSOLA:
			// TODO: implementar la logica cuando recibe este protocolo
			enviar_respuesta_a_consola(datos_cliente->socket, FINALIZAR_CONSOLA_OK);
			break;
		default:
			log_error(logger,"Protocolo invalido.");
			break;
	}
}

void enviar_respuesta_a_consola(int socket_fd, t_protocolo protocolo) {
	enviar_datos(socket_fd, &protocolo, sizeof(t_protocolo));
}

