#ifndef KERNEL_GLOBAL_H_
#define KERNEL_GLOBAL_H_

#include <commons/log.h>

t_log *kernel_logger;


pthread_mutex_t planificador_mutex_new;
pthread_mutex_t planificador_mutex_ready;
pthread_mutex_t planificador_mutex_exec;
pthread_mutex_t planificador_mutex_blocked;
pthread_mutex_t planificador_mutex_suspended_blocked;
pthread_mutex_t planificador_mutex_suspended_ready;
pthread_mutex_t planificador_mutex_fin;

#endif /* KERNEL_GLOBAL_H_ */
