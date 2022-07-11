#ifndef CPU_GLOBAL_H_
#define CPU_GLOBAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/log.h>
#include <protocolo.h>
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
uint8_t ultimo_en_ejecutar;


#endif /* CPU_GLOBAL_H_ */
