/*
 * gestionar_consola.h
 *
 *  Created on: 20 abr. 2022
 *      Author: utnso
 */

#ifndef GESTIONAR_CONSOLA_H_
#define GESTIONAR_CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <sockets.h>
#include <servidor.h>
#include <protocolo.h>

void procesar_operacion(t_cliente *datos_cliente);
void enviar_respuesta_a_consola(int socket_fd, t_protocolo protocolo);

#endif /* GESTIONAR_CONSOLA_H_ */
