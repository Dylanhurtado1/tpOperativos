#ifndef CPU_GLOBAL_H_
#define CPU_GLOBAL_H_


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/log.h>
#include <protocolo.h>
#include <errors.h>
#include <estructuras.h>
#include <conexiones.h>
#include <serializador.h>
#include "cpu_config.h"

t_log *cpu_logger;
int socket_memoria;
t_cpu_config *cpu_config;
t_traductor *traductor;
pthread_mutex_t mutex_interrupt;
bool interrupcion_desalojo;
t_list *tlb;


#endif /* CPU_GLOBAL_H_ */
