#ifndef KERNEL_GLOBAL_H_
#define KERNEL_GLOBAL_H_

#include <commons/log.h>
#include <pthread.h>
#include <semaphore.h>

t_log *kernel_logger;


// Planificador Largo Plazo
pthread_mutex_t planificador_mutex_new;
pthread_mutex_t plp_mutex_generador_id;


// Planificador Corto Plazo
sem_t sem_ready;


pthread_mutex_t planificador_mutex_ready;
pthread_mutex_t planificador_mutex_exec;
pthread_mutex_t planificador_mutex_blocked;
pthread_mutex_t planificador_mutex_suspended_blocked;
pthread_mutex_t planificador_mutex_suspended_ready;
pthread_mutex_t planificador_mutex_fin;

#endif /* KERNEL_GLOBAL_H_ */
