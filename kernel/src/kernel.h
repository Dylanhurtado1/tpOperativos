/*
 * kernel.h
 *
 *  Created on: 19 abr. 2022
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <protocolo.h>
#include <conexiones.h>
#include "kernel_config.h"
#include "gestionar_consola.h"
#include "planificador.h"

#define PATH_KERNEL_CONFIG "/home/utnso/tp-2022-1c-lo-importante-es-aprobar/kernel/kernel.config"


//KERNEL

void iniciar_colas_de_planificacion();
void inicio_estructuras_planificacion();
void inicio_planificacion();


//HILOS

void iniciar_hilo_cola_new();
void iniciar_hilo_cola_ready();
void iniciar_hilo_cola_blocked();
void iniciar_hilo_cola_suspended_blocked();
void iniciar_hilo_cola_suspended_ready();


//FUNCIONES PARA PLANIFICAR CADA COLA

void planificar_cola_ready();
void planificar_cola_block();
void planificar_cola_suspended_blocked();
void planificar_cola_suspended_ready();
void planificar_cola_exec();


//SEMAFOROS

//pthread_mutex_t planificador_mutex_new;
pthread_mutex_t planificador_mutex_ready;
pthread_mutex_t planificador_mutex_exec;
pthread_mutex_t planificador_mutex_blocked;
pthread_mutex_t planificador_mutex_suspended_blocked;
pthread_mutex_t planificador_mutex_suspended_ready;
//pthread_mutex_t planificador_mutex_fin;


#endif /* KERNEL_H_ */
