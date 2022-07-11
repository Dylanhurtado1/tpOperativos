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

void transicion_suspender(t_proceso *proceso) {//mete un proceso a la cola suspended_blocked
	log_info(kernel_logger, "PID[%d] ingresa a SUSPENDED-BLOCKED", proceso->pcb->id);
	proceso->estado = SUSPENDED_BLOCKED;
	enviar_proceso_a_memoria(proceso, socket_memoria, SUSPENSION_DE_PROCESO);//se envia a memoria el proceso
	t_protocolo protocolo = esperar_respuesta_memoria(socket_memoria);//espera respuesta de memoria
	if(protocolo != PROCESO_SUSPENDIDO) {
		log_error(kernel_logger, "No se pudo liberar la memoria de PID[%d]", proceso->pcb->id);
	}//si es suspended_blocked
	pthread_mutex_lock(&mutex_suspended_blocked);
	queue_push(cola_suspended_blocked, proceso);//pone el proceso a la cola susp-blocked
	pthread_mutex_unlock(&mutex_suspended_blocked);

	sem_post(&sem_grado_multiprogramacion);//y se libera un espacio para poner a la cola de ready
}

void estado_suspended_ready(void *data) {
	while(1) {
		sem_wait(&sem_suspended_ready);//espera el post
		pthread_mutex_lock(&mutex_suspended_blocked);
		t_proceso *proceso = queue_pop(cola_suspended_blocked);//saca el procesos de la cola
		pthread_mutex_unlock(&mutex_suspended_blocked);

		pthread_mutex_lock(&mutex_suspended_ready);
		queue_push(cola_suspended_ready, proceso);//mete a susp-ready
		pthread_mutex_unlock(&mutex_suspended_ready);

		log_info(kernel_logger, "PID[%d] ingresa a SUSPENDED-READY...", proceso->pcb->id);
		sem_post(&sem_admitir);//un post nose a quien deah
	}
}


