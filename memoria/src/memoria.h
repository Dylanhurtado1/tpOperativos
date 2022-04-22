/*
 * memoria.h
 *
 *  Created on: 19 abr. 2022
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <servidor.h>
#include <sockets.h>
#include <protocolo.h>
#include <estructuras.h>
#include "memoria_config.h"

#define PATH_MEMORIA_CONFIG "/home/utnso/tp-2022-1c-lo-importante-es-aprobar/memoria/memoria.config"

int iniciar_servidor_memoria(char *ip, char *puerto);
void procesar_conexiones(t_cliente *datos_cliente);
void enviar_numero_tabla_de_pagina(int socket_fd, uint32_t numero);
void enviar_estructura_traductora(int socket_fd, t_traductor *traductor);
void eliminar_traductor(t_traductor *traductor);

#endif /* MEMORIA_H_ */
