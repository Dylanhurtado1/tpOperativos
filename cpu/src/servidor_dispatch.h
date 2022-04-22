/*
 * servidor_dispatch.h
 *
 *  Created on: 22 abr. 2022
 *      Author: utnso
 */

#ifndef SERVIDOR_DISPATCH_H_
#define SERVIDOR_DISPATCH_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <cliente.h>
#include <servidor.h>
#include <sockets.h>
#include <protocolo.h>
#include <errors.h>
#include <estructuras.h>

int iniciar_servidor_dispatch(char *ip, char *puerto);
void peticiones_dispatch(int *socket_fd);

#endif /* SERVIDOR_DISPATCH_H_ */
