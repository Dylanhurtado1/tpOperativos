/*
 ============================================================================
 Name        : memoria.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <cliente.h>
#include <sockets.h>
#include <commons/log.h>

int main(void) {
	t_log *logger = log_create("memoria.log", "SERVER", true, LOG_LEVEL_INFO);
	int server_fd = conectar_a_servidor("", "");

	log_info(logger, "Memoria iniciado");

	log_destroy(logger);
	cerrar_conexion(server_fd);

	return EXIT_SUCCESS;
}
