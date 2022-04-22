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
#include <pthread.h>
#include <commons/log.h>
#include <cliente.h>
#include <servidor.h>
#include <sockets.h>
#include <protocolo.h>
#include <errors.h>
#include <estructuras.h>
#include <conexiones.h>
#include "cpu_config.h"
#include "peticiones.h"
//#include "servidor_dispatch.h"
//#include "servidor_interrupt.h"

#define PATH_CPU_CONFIG "/home/utnso/tp-2022-1c-lo-importante-es-aprobar/cpu/cpu.config"

t_traductor *obtener_traductor_direcciones(int socket_fd);
void realizar_handshake(int socket_fd);
t_traductor *deserealizar_traductor(t_paquete *paquete);
void eliminar_traductor_direcciones(t_traductor *traductor);

#endif /* CPU_H_ */
