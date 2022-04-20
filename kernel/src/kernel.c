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

void procesar_operacion(t_paquete *paquete);

t_log *logger;

int main(void) {
	logger = log_create("kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
	t_kernel_config *config = kernel_leer_configuracion(PATH_KERNEL_CONFIG);

	int server_fd = crear_servidor(config->ip_kernel, config->puerto_escucha);
	if(server_fd == INIT_SERVER_ERROR) {
		log_error(logger, "No se pudo iniciar el servidor");
		return EXIT_FAILURE;
	}

	if(atender_clientes(server_fd, procesar_operacion) == WAIT_CLIENT_ERROR) {
		log_error(logger, "Error al escuchar clientes... Finalizando servidor");
	}


	log_destroy(logger);
	kernel_eliminar_configuracion(config);
	cerrar_conexion(server_fd);

	return EXIT_SUCCESS;
}

void procesar_operacion(t_paquete *paquete) {
	char *mensaje;
	t_list* lista;
	t_protocolo respuesta;
	switch (paquete->codigo_operacion) {
		case DEBUG_MENSAJE:
			mensaje = deserealizar_mensaje(paquete);
			log_info(logger,"Mensaje recibido: %s", mensaje);
			free(mensaje);
			break;
		case DEBUG_PAQUETE:
			lista = deserealizar_paquete(paquete);
			log_info(logger,"Tamanio de datos recibidos: %d", list_size(lista));
			list_destroy_and_destroy_elements(lista, free);
			break;
		case DATOS_CONSOLA:
			mensaje = deserealizar_mensaje(paquete);
			log_info(logger,"Mensaje recibido: %s", mensaje);
			free(mensaje);
			// TODO: implementar la logica cuando recibe este protocolo
			respuesta = FINALIZAR_CONSOLA_OK;
			enviar_datos(server_fd, &respuesta, sizeof(t_protocolo));
			break;
		default:
			log_error(logger,"Protocolo invalido.");
			break;
	}
}
