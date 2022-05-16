#ifndef KERNEL_GLOBAL_H_
#define KERNEL_GLOBAL_H_

#include <commons/log.h>
#include <pthread.h>
#include <semaphore.h>

// Kernel
t_log *kernel_logger;
t_kernel_config *kernel_config;
int socket_cpu_interrupt;
int socket_cpu_dispatch;
int socket_memoria;

// Planificador Largo Plazo
uint32_t generador_de_id;
uint32_t procesos_admitidos_en_ready;
t_queue *cola_new;
t_queue *cola_exit;
sem_t sem_exit;
sem_t sem_grado_multiprogramacion;
pthread_mutex_t mutex_generador_id;
pthread_mutex_t mutex_new;
pthread_mutex_t mutex_exit;
pthread_t thread_exit;


// Planificador Corto Plazo
uint32_t *tiempo_bloqueo;
t_queue *cola_ready;
t_queue *cola_exec;
t_queue *cola_blocked;
sem_t sem_ready;
sem_t sem_exec;
sem_t sem_blocked;
pthread_mutex_t mutex_ready;
pthread_mutex_t mutex_blocked;
pthread_mutex_t mutex_exec;
pthread_t thread_ready;
pthread_t thread_exec;
pthread_t thread_blocked;




pthread_mutex_t planificador_mutex_exec;
pthread_mutex_t planificador_mutex_blocked;
pthread_mutex_t planificador_mutex_suspended_blocked;
pthread_mutex_t planificador_mutex_suspended_ready;
pthread_mutex_t planificador_mutex_fin;

#endif /* KERNEL_GLOBAL_H_ */
