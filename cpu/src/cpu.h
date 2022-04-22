/*
 * cpu.h
 *
 *  Created on: 19 abr. 2022
 *      Author: utnso
 */

#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <cliente.h>
#include <servidor.h>
#include <sockets.h>
#include <protocolo.h>
#include <errors.h>
#include <estructuras.h>
#include "cpu_config.h"

#define PATH_CPU_CONFIG "/home/utnso/tp-2022-1c-lo-importante-es-aprobar/cpu/cpu.config"

int conectar_a_memoria(char *ip, char *puerto);
int iniciar_servidor_dispatch(char *ip, char *puerto);
int iniciar_servidor_interrupt(char *ip, char *puerto);
t_traductor *obtener_traductor_direcciones(int socket_fd);
void realizar_handshake(int socket_fd);
t_traductor *deserealizar_traductor(t_paquete *paquete);
void eliminar_traductor_direcciones(t_traductor *traductor);

#endif /* CPU_H_ */
