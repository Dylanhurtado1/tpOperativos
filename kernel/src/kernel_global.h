#ifndef KERNEL_GLOBAL_H_
#define KERNEL_GLOBAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <protocolo.h>
#include <estructuras.h>
#include <serializador.h>
#include <conexiones.h>
#include "kernel_config.h"

// Kernel
t_log *kernel_logger;//global
t_kernel_config *kernel_config;//variable con todos los atributos del config
int socket_cpu_interrupt;
int socket_cpu_dispatch;
int socket_memoria;

// Planificador Largo Plazo
uint32_t generador_de_id;
//uint32_t procesos_admitidos_en_ready;
t_queue *cola_new;
t_queue *cola_exit;
sem_t sem_admitir;
sem_t sem_exit;
sem_t sem_grado_multiprogramacion;
pthread_mutex_t mutex_generador_id;
pthread_mutex_t mutex_new;
pthread_mutex_t mutex_exit;
pthread_t thread_exit;
pthread_t thread_admitir;


// Planificador Corto Plazo
t_list *cola_ready;
t_queue *cola_exec;
t_queue *cola_blocked;
sem_t sem_ready;
sem_t sem_exec;
sem_t sem_blocked;
sem_t sem_desalojo;
pthread_mutex_t mutex_ready;
pthread_mutex_t mutex_blocked;
pthread_mutex_t mutex_exec;
pthread_t thread_ready;
pthread_t thread_exec;
pthread_t thread_blocked;


// Planificador Mediano Plazo
t_queue *cola_suspended_blocked;
t_queue *cola_suspended_ready;
//sem_t sem_suspended_blocked;
sem_t sem_suspended_ready;
pthread_mutex_t mutex_suspended_blocked;
pthread_mutex_t mutex_suspended_ready;
pthread_t thread_suspended_ready;


#endif /* KERNEL_GLOBAL_H_ */
