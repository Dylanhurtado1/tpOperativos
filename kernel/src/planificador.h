/*
 * planificador.h
 *
 *  Created on: 21 abr. 2022
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <sockets.h>
#include <servidor.h>
#include <protocolo.h>
#include <comunicacion.h>
#include <estructuras.h>
#include <serializador.h>
#include "kernel_config.h"

// Planificador de corto plazo
void agregar_proceso_a_ready(t_pcb *proceso);
void iniciar_cola_ready();
void eliminar_cola_ready();
void ejecutar_proceso();
bool hay_proceso_en_ejecucion();

// Planificador de largo plazo
void agregar_proceso_a_new(t_list *instrucciones, uint32_t tam_proceso);
bool es_posible_admitir_proceso();
void admitir_proceso();
void iniciar_cola_new();
void eliminar_cola_new();


#endif /* PLANIFICADOR_H_ */
