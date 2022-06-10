#include "planificador.h"


void iniciar_planificador_mediano_plazo() {
	pthread_mutex_init(&mutex_suspended_ready, NULL);
	pthread_mutex_init(&mutex_suspended_blocked, NULL);
	sem_init(&sem_suspended_ready, 0, 0);
	//sem_init(&sem_suspended_blocked, 0, 0);
	cola_suspended_blocked = queue_create();
	cola_suspended_ready = queue_create();
	pthread_create(&thread_suspended_ready, NULL, (void *)estado_suspended_ready, NULL);
	pthread_detach(thread_suspended_ready);
}

void transicion_suspender(t_proceso *proceso) {
	log_info(kernel_logger, "PID[%d] ingresa a SUSPENDED-BLOCKED", proceso->pcb->id);
	proceso->estado = SUSPENDED_BLOCKED;
	enviar_proceso_a_memoria(proceso, socket_memoria, SUSPENSION_DE_PROCESO);
	t_protocolo protocolo = esperar_respuesta_memoria(socket_memoria);
	if(protocolo != PROCESO_SUSPENDIDO) {
		log_error(kernel_logger, "No se pudo liberar la memoria de PID[%d]", proceso->pcb->id);
	}
	pthread_mutex_lock(&mutex_suspended_blocked);
	queue_push(cola_suspended_blocked, proceso);
	pthread_mutex_unlock(&mutex_suspended_blocked);

	sem_post(&sem_grado_multiprogramacion);
}

void estado_suspended_ready(void *data) {
	while(1) {
		sem_wait(&sem_suspended_ready);
		pthread_mutex_lock(&mutex_suspended_blocked);
		t_proceso *proceso = queue_pop(cola_suspended_blocked);
		pthread_mutex_unlock(&mutex_suspended_blocked);

		pthread_mutex_lock(&mutex_suspended_ready);
		queue_push(cola_suspended_ready, proceso);
		pthread_mutex_unlock(&mutex_suspended_ready);

		log_info(kernel_logger, "PID[%d] ingresa a SUSPENDED-READY...", proceso->pcb->id);
		sem_post(&sem_admitir);
	}
}


