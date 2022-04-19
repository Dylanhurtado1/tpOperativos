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

int main(void) {
	t_log *logger = log_create("kernel.log", "KERNEL", true, LOG_LEVEL_INFO);

	t_kernel_config *config = kernel_leer_configuracion(PATH_KERNEL_CONFIG);

	int server_fd = iniciar_servidor(config->ip_kernel, config->puerto_escucha);

	int cliente_fd = esperar_cliente(server_fd);

	log_destroy(logger);
	kernel_eliminar_configuracion(config);
	cerrar_conexion(server_fd);

	return EXIT_SUCCESS;
}
