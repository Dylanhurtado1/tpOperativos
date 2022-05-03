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
#include <protocolo.h>
#include <conexiones.h>
#include <estructuras.h>
#include <serializador.h>
#include "memoria_config.h"

#define PATH_MEMORIA_CONFIG "/home/utnso/tp-2022-1c-lo-importante-es-aprobar/memoria/memoria.config"

void procesar_conexiones(t_cliente *datos_cliente);
void enviar_numero_tabla_de_pagina(int socket_fd, uint32_t numero);
void enviar_estructura_traductora(int socket_fd, t_traductor *traductor);
t_traductor *crear_traductor(int entradas_tabla, int tamanio_pagina);
void eliminar_traductor(t_traductor *traductor);

#endif /* MEMORIA_H_ */
