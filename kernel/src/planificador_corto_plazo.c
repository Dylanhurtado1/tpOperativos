#include "planificador.h"
#include "temporizador.h"

uint32_t diferencia_absoluta(uint32_t tiempo_1, uint32_t tiempo_2);
void io(uint32_t tiempo);
double calcular_estimacion_rafaga(uint32_t tiempo_ejecucion, double tiempo_estimado);

bool proceso_ejecutando;


void iniciar_planificador_corto_plazo() {
	pthread_mutex_init(&mutex_ready, NULL);
	pthread_mutex_init(&mutex_blocked, NULL);
	pthread_mutex_init(&mutex_exec, NULL);
	sem_init(&sem_ready, 0, 0);
	sem_init(&sem_exec, 0, 0);
	sem_init(&sem_blocked, 0, 0);
	sem_init(&sem_desalojo, 0, 0);
	cola_ready = list_create();
	cola_exec = queue_create();
	cola_blocked = queue_create();
	pthread_create(&thread_ready, NULL, (void *)estado_ready, NULL);
	pthread_create(&thread_exec, NULL, (void *)estado_exec, NULL);
	pthread_create(&thread_blocked, NULL, (void *)estado_blocked, NULL);
	pthread_detach(thread_ready);
	pthread_detach(thread_exec);
	pthread_detach(thread_blocked);

	iniciar_timer();
	proceso_ejecutando = false;
}

void estado_ready(void *data) {
	while(1) {
		sem_wait(&sem_ready);

		if(string_equals_ignore_case(kernel_config->algoritmo_planificacion, "SRT")) {
			pthread_mutex_lock(&mutex_exec);
			if(proceso_ejecutando) {
				pthread_mutex_unlock(&mutex_exec);
				enviar_interrupcion_a_cpu(socket_cpu_interrupt);
				sem_wait(&sem_desalojo);
			} else {
				pthread_mutex_unlock(&mutex_exec);
			}
		}

		t_pcb *proceso = siguiente_a_ejecutar(kernel_config->algoritmo_planificacion);

		pthread_mutex_lock(&mutex_exec);
		queue_push(cola_exec, proceso);
		pthread_mutex_unlock(&mutex_exec);

		sem_post(&sem_exec);
	}
}

void estado_exec(void *data) {
	while(1) {
		sem_wait(&sem_exec);
		pthread_mutex_lock(&mutex_exec);
		t_pcb *proceso = queue_pop(cola_exec);
		proceso_ejecutando = true;
		pthread_mutex_unlock(&mutex_exec);

		uint32_t tiempo_inicio_cpu = get_tiempo_actual();
		enviar_proceso_a_cpu(proceso, socket_cpu_dispatch);
		eliminar_proceso(proceso);
		t_paquete *paquete = esperar_respuesta_cpu(socket_cpu_dispatch);
		uint32_t tiempo_ejecucion = get_tiempo_actual() - tiempo_inicio_cpu;
		pthread_mutex_lock(&mutex_exec);
		proceso_ejecutando = false;
		pthread_mutex_unlock(&mutex_exec);

		t_pcb *pcb = deserializar_pcb(paquete);
		pcb->estimacion_rafaga = calcular_estimacion_rafaga(tiempo_ejecucion, pcb->estimacion_rafaga);
		log_info(kernel_logger, "PID[%d] -> tiempo ejecucion = %d, nueva estimacion = %d",
				pcb->id, tiempo_ejecucion, pcb->estimacion_rafaga);

		switch(paquete->codigo_operacion) {
			case BLOQUEAR_PROCESO:
				log_info(kernel_logger, "Proceso ejecuto I/O, enviando a cola de bloqueo...");
				pthread_mutex_lock(&mutex_blocked);
				pcb->estado = BLOCKED;
				pcb->tiempo_inicio_bloqueo = get_tiempo_actual();
				queue_push(cola_blocked, pcb);
				pthread_mutex_unlock(&mutex_blocked);

				sem_post(&sem_blocked);
				break;
			case FINALIZAR_PROCESO:
				log_info(kernel_logger, "Proceso ejecuto EXIT, enviando a cola de salida...");
				pthread_mutex_lock(&mutex_exit);
				queue_push(cola_exit, pcb);
				pthread_mutex_unlock(&mutex_exit);

				sem_post(&sem_exit);
				break;
			case PROCESO_DESALOJADO:
				log_info(kernel_logger, "Proceso desalojado por interrupcion, seleccionar siguiente proceso a ejecutar...");
				pthread_mutex_lock(&mutex_ready);
				list_add(cola_ready, pcb);
				pthread_mutex_unlock(&mutex_ready);

				sem_post(&sem_desalojo);
				sem_post(&sem_ready);
				break;
			default:
				log_error(kernel_logger, "Operacion desconocida.");
				break;
		}

		eliminar_paquete(paquete);
	}
}

void estado_blocked(void *data) {
	while(1) {
		sem_wait(&sem_blocked);
		pthread_mutex_lock(&mutex_blocked);
		t_pcb *pcb = queue_pop(cola_blocked);
		pthread_mutex_unlock(&mutex_blocked);

		uint32_t tiempo_bloqueado = get_tiempo_actual() - pcb->tiempo_inicio_bloqueo;

		if(tiempo_bloqueado > kernel_config->tiempo_maximo_bloqueado) {
			transicion_suspender(pcb);
			io(pcb->tiempo_io);

			sem_post(&sem_suspended_ready);
		} else if(tiempo_bloqueado + pcb->tiempo_io > kernel_config->tiempo_maximo_bloqueado) {
			uint32_t tiempo_io_antes_suspender = kernel_config->tiempo_maximo_bloqueado - tiempo_bloqueado;
			io(tiempo_io_antes_suspender);
			transicion_suspender(pcb);
			io(diferencia_absoluta(tiempo_io_antes_suspender, pcb->tiempo_io));

			sem_post(&sem_suspended_ready);
		} else {
			log_info(kernel_logger, "Proceso quedo BLOCKED...");
			io(pcb->tiempo_io);
			pthread_mutex_lock(&mutex_ready);
			list_add(cola_ready, pcb);
			pthread_mutex_unlock(&mutex_ready);

			sem_post(&sem_ready);
		}

		log_info(kernel_logger, "[IO] -> PID = %d, tiempo io = %d, tiempo bloqueado = %d...",
				pcb->id, pcb->tiempo_io, tiempo_bloqueado);
	}
}

void io(uint32_t tiempo) {
	usleep(tiempo * 1000);
}

uint32_t diferencia_absoluta(uint32_t tiempo_1, uint32_t tiempo_2) {
	return tiempo_1 > tiempo_2 ? tiempo_1 - tiempo_2 : tiempo_2 - tiempo_1;
}

double calcular_estimacion_rafaga(uint32_t tiempo_ejecucion, double tiempo_estimado) {
	return kernel_config->alfa * tiempo_ejecucion + (1 - kernel_config->alfa) * tiempo_estimado;
}

void enviar_proceso_a_cpu(t_pcb *pcb, int socket_cpu_dispatch) {
	t_paquete *paquete = serializar_pcb(pcb, PCB);
	enviar_paquete(paquete, socket_cpu_dispatch);
	eliminar_paquete(paquete);
}

t_paquete *esperar_respuesta_cpu(int socket_cpu_dispatch) {
	return recibir_paquete(socket_cpu_dispatch);
}

void enviar_interrupcion_a_cpu(int socket_fd){
	log_info(kernel_logger, "Enviando interrupcion de desalojo...");
	t_paquete *paquete = crear_paquete(DESALOJAR_PROCESO, buffer_vacio());
	uint32_t nada = 0;
	agregar_a_paquete(paquete, &nada, sizeof(uint32_t));
	enviar_paquete(paquete, socket_fd);
	eliminar_paquete(paquete);
}

t_pcb *siguiente_a_ejecutar(char *algoritmo) {
	bool menor_rafaga(void *pcb1, void *pcb2) {
		return ((t_pcb *)pcb1)->estimacion_rafaga < ((t_pcb *)pcb2)->estimacion_rafaga;
	}

	t_pcb *pcb = NULL;
	pthread_mutex_lock(&mutex_ready);
	if(string_equals_ignore_case(algoritmo, "SRT")) {
		list_sort(cola_ready, menor_rafaga);
	}
	else if(string_equals_ignore_case(algoritmo, "FIFO")) {
	}

	pcb = list_remove(cola_ready, 0);
	pthread_mutex_unlock(&mutex_ready);

	return pcb;
}






