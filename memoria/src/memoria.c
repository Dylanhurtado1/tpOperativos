/*
 ============================================================================
 Name        : memoria.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "memoria.h"

#define PATH_MEMORIA_CONFIG "/home/utnso/tp-2022-1c-lo-importante-es-aprobar/memoria/memoria.config"

int main(void) {
	t_log *logger = log_create("memoria.log", "MEMORIA", true, LOG_LEVEL_INFO);
	t_memoria_config *config = memoria_leer_configuracion(PATH_MEMORIA_CONFIG);

	int socket_memoria = crear_servidor(config->ip_memoria, config->puerto_escucha);
	if(socket_memoria == INIT_SERVER_ERROR) {
		log_error(logger, "No se pudo iniciar el servidor Memoria");
		return EXIT_FAILURE;
	}

	log_destroy(logger);
	memoria_eliminar_configuracion(config);
	cerrar_conexion(socket_memoria);

	return EXIT_SUCCESS;
}
