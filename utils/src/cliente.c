/*
 * cliente.c
 *
 *  Created on: 25 mar. 2022
 *      Author: utnso
 */


#include "sockets.h"
#include "errors.h"

int conectar_a_servidor(char *ip, char *puerto) {
	int server_fd = crear_conexion(ip, puerto);
	if(es_codigo_de_error(server_fd)) {
		return SERVER_CONNECTION_ERROR;
	}
	return server_fd;
}
