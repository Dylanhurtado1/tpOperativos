/*
 * servidor.h
 *
 *  Created on: 22 mar. 2022
 *      Author: utnso
 */

#ifndef SERVIDOR_H_
#define SERVIDOR_H_

#include <stdbool.h>
#include <pthread.h>
#include "sockets.h"
#include "comunicacion.h"
#include "paquete.h"
#include "errors.h"

typedef struct {
	int socket;
	t_paquete *paquete;
} t_cliente;

typedef struct {
	int socket;
	void (*callback)(t_cliente *);
} t_socket;


int crear_servidor(char *ip, char *puerto);
int atender_clientes(int socket_servidor, void (*callback)(t_cliente *));

#endif /* SERVIDOR_H_ */
