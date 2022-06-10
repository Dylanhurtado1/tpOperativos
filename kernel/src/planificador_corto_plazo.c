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
	sem_init(&sem_desalojo, 0, 1);
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
				enviar_interrupcion_a_cpu(socket_cpu_interrupt);
			}
			pthread_mutex_unlock(&mutex_exec);
		}

		sem_wait(&sem_desalojo);
		t_proceso *proceso = siguiente_a_ejecutar(kernel_config->algoritmo_planificacion);

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
		t_proceso *proceso = queue_pop(cola_exec);
		proceso_ejecutando = true;
		pthread_mutex_unlock(&mutex_exec);
		log_info(kernel_logger, "PID[%d] ingresa a EXEC", proceso->pcb->id);

		uint32_t tiempo_inicio_cpu = get_tiempo_actual();
		enviar_proceso_a_cpu(proceso, socket_cpu_dispatch);
		eliminar_pcb(proceso->pcb);
		t_paquete *paquete = esperar_respuesta_cpu(socket_cpu_dispatch);
		proceso->tiempo_cpu += get_tiempo_actual() - tiempo_inicio_cpu;
		pthread_mutex_lock(&mutex_exec);
		proceso_ejecutando = false;
		pthread_mutex_unlock(&mutex_exec);

		proceso->pcb = deserializar_pcb(paquete);

		switch(paquete->codigo_operacion) {
			case DESALOJO_POR_IO:
				recibir_datos(socket_cpu_dispatch, &(proceso->tiempo_io), sizeof(uint32_t));
				pthread_mutex_lock(&mutex_blocked);
				proceso->estado = BLOCKED;
				proceso->tiempo_inicio_bloqueo = get_tiempo_actual();
				proceso->pcb->estimacion_rafaga = calcular_estimacion_rafaga(proceso->tiempo_cpu, proceso->pcb->estimacion_rafaga);
				proceso->tiempo_cpu = 0;
				queue_push(cola_blocked, proceso);
				pthread_mutex_unlock(&mutex_blocked);

				sem_post(&sem_blocked);
				break;
			case DESALOJO_POR_EXIT:
				pthread_mutex_lock(&mutex_exit);
				queue_push(cola_exit, proceso);
				pthread_mutex_unlock(&mutex_exit);

				sem_post(&sem_exit);
				break;
			case DESALOJO_POR_IRQ:
				pthread_mutex_lock(&mutex_ready);
				list_add(cola_ready, proceso);
				pthread_mutex_unlock(&mutex_ready);

				sem_post(&sem_ready);
				break;
			default:
				log_error(kernel_logger, "Protocolo invalido.");
				break;
		}

		sem_post(&sem_desalojo);
		eliminar_paquete(paquete);
	}
}

void estado_blocked(void *data) {
	while(1) {
		sem_wait(&sem_blocked);
		pthread_mutex_lock(&mutex_blocked);
		t_proceso *proceso = queue_pop(cola_blocked);
		pthread_mutex_unlock(&mutex_blocked);
		log_info(kernel_logger, "PID[%d] ingresa a BLOCKED", proceso->pcb->id);

		uint32_t tiempo_bloqueado = get_tiempo_actual() - proceso->tiempo_inicio_bloqueo;

		if(tiempo_bloqueado > kernel_config->tiempo_maximo_bloqueado) {
			transicion_suspender(proceso);
			io(proceso->tiempo_io);

			sem_post(&sem_suspended_ready);
		} else if(tiempo_bloqueado + proceso->tiempo_io > kernel_config->tiempo_maximo_bloqueado) {
			uint32_t tiempo_io_antes_suspender = kernel_config->tiempo_maximo_bloqueado - tiempo_bloqueado;
			io(tiempo_io_antes_suspender);
			transicion_suspender(proceso);
			io(diferencia_absoluta(tiempo_io_antes_suspender, proceso->tiempo_io));

			sem_post(&sem_suspended_ready);
		} else {
			io(proceso->tiempo_io);
			pthread_mutex_lock(&mutex_ready);
			list_add(cola_ready, proceso);
			pthread_mutex_unlock(&mutex_ready);

			sem_post(&sem_ready);
		}

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

void enviar_proceso_a_cpu(t_proceso *proceso, int socket_cpu_dispatch) {
	t_paquete *paquete = serializar_pcb(proceso->pcb, PCB);
	enviar_paquete(paquete, socket_cpu_dispatch);
	eliminar_paquete(paquete);
}

t_paquete *esperar_respuesta_cpu(int socket_cpu_dispatch) {
	return recibir_paquete(socket_cpu_dispatch);
}

void enviar_interrupcion_a_cpu(int socket_fd){
	t_paquete *paquete = crear_paquete(DESALOJAR_PROCESO, buffer_vacio());
	uint32_t nada = 0;
	agregar_a_paquete(paquete, &nada, sizeof(uint32_t));
	enviar_paquete(paquete, socket_fd);
	eliminar_paquete(paquete);
}

t_proceso *siguiente_a_ejecutar(char *algoritmo) {
	t_proceso *proceso = NULL;

	bool menor_rafaga(void *p1, void *p2) {
		uint32_t estimacion_p1 = ((t_proceso *)p1)->pcb->estimacion_rafaga - ((t_proceso *)p1)->tiempo_cpu;
		uint32_t estimacion_p2 = ((t_proceso *)p2)->pcb->estimacion_rafaga - ((t_proceso *)p2)->tiempo_cpu;
		return estimacion_p1 <= estimacion_p2;
	}

	pthread_mutex_lock(&mutex_ready);
	if(string_equals_ignore_case(algoritmo, "SRT")) {
		list_sort(cola_ready, menor_rafaga);
	}
	else if(string_equals_ignore_case(algoritmo, "FIFO")) {
	}

	proceso = list_remove(cola_ready, 0);
	pthread_mutex_unlock(&mutex_ready);

	return proceso;
}

void eliminar_pcb(t_pcb *pcb) {
	list_destroy_and_destroy_elements(pcb->instrucciones, free);
	free(pcb);
}





