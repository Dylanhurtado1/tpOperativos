/*
 ============================================================================
 Name        : consola.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "consola.h"

#define PATH_CONSOLA_CONFIG "/home/utnso/tp-2022-1c-lo-importante-es-aprobar/consola/consola.config"


int main(int argc, char **argv) {
	t_log *logger = log_create("consola.log", "CONSOLA", true, LOG_LEVEL_INFO);

	if(argc < 2) {
		log_error(logger, "Error de parametros. Ejemplo de uso: ./consola <ruta_a_pseudocodigo> <tam_proceso>");
		log_destroy(logger);
		return EXIT_FAILURE;
	}

	parsear_instrucciones(argv[1]);

	t_consola_config *config = consola_leer_configuracion(PATH_CONSOLA_CONFIG);

	int server_fd = conectar_a_servidor(config->ip_kernel, config->puerto_kernel);
	if(server_fd == SERVER_CONNECTION_ERROR) {
		log_error(logger, "Error al conectar con el servidor");
	}

	enviar_mensaje("hola cesar", DEBUG_MENSAJE, server_fd);

	log_destroy(logger);
	consola_eliminar_configuracion(config);
	cerrar_conexion(server_fd);

	return EXIT_SUCCESS;

}



