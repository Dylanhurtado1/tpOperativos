#include "planificador.h"
#include "temporizador.h"

static uint32_t diferencia_absoluta(uint32_t tiempo_1, uint32_t tiempo_2);
static void io(uint32_t tiempo);
static double calcular_estimacion_rafaga(uint32_t tiempo_ejecucion, double tiempo_estimado);

bool proceso_ejecutando;


void iniciar_planificador_corto_plazo() {
	pthread_mutex_init(&mutex_ready, NULL);
	pthread_mutex_init(&mutex_blocked, NULL);
	pthread_mutex_init(&mutex_exec, NULL);
	sem_init(&sem_ready, 0, 0);
	sem_init(&sem_exec, 0, 0);
	sem_init(&sem_blocked, 0, 0);
	sem_init(&sem_desalojo, 0, 1);
	cola_ready = list_create();//lista de ready
	cola_exec = queue_create();//Cola exec
	cola_blocked = queue_create();//Cola blocked
	pthread_create(&thread_ready, NULL, (void *)estado_ready, NULL);//READY -> EXEC
	pthread_create(&thread_exec, NULL, (void *)estado_exec, NULL);//EXEC -> BLOCKED,->READY,O EXIT
	pthread_create(&thread_blocked, NULL, (void *)estado_blocked, NULL);//BLOCKED -> SUSP-BLOCKED,O -> READY
	pthread_detach(thread_ready);
	pthread_detach(thread_exec);
	pthread_detach(thread_blocked);

	iniciar_timer();//esto no lo entiendo
	proceso_ejecutando = false;
}

void estado_ready(void *data) {
	while(1) {
		sem_wait(&sem_ready);//espera el post del plani de largo plazo

		if(string_equals_ignore_case(kernel_config->algoritmo_planificacion, "SRT")) {//si es srt
			pthread_mutex_lock(&mutex_exec);
			if(proceso_ejecutando) {//si hay un proceso en la cola exec
				enviar_interrupcion_a_cpu(socket_cpu_interrupt, DESALOJAR_PROCESO);//se envia interrupcion a cpu,para desalojar un proceso
			}//Si no hay procesos ejecutando
			pthread_mutex_unlock(&mutex_exec);
		}

		sem_wait(&sem_desalojo);//espera el post desde cpu
		t_proceso *proceso = siguiente_a_ejecutar(kernel_config->algoritmo_planificacion);//retorna un proceso de la cola ready para pasar a exec

		pthread_mutex_lock(&mutex_exec);
		queue_push(cola_exec, proceso);//mete al proceso a la cola exec
		pthread_mutex_unlock(&mutex_exec);

		sem_post(&sem_exec);//indica que hay un proceso en la cola exec
	}
}

void estado_exec(void *data) {
	while(1) {
		sem_wait(&sem_exec);//espera el post del plani largo plazo
		pthread_mutex_lock(&mutex_exec);
		t_proceso *proceso = queue_pop(cola_exec);//saca un proceso de cola exec
		proceso_ejecutando = true;//variable global en true
		pthread_mutex_unlock(&mutex_exec);
		log_info(kernel_logger, "PID[%d] ingresa a EXEC", proceso->pcb->id);

		uint32_t tiempo_inicio_cpu = get_tiempo_actual();//obtiene un tiempo que empieza
		enviar_proceso_a_cpu(proceso, socket_cpu_dispatch);//se envia a cpu
		eliminar_pcb(proceso->pcb);//se elimina el pcb, porque el nuevo pcb es el recibido por cpu
		t_paquete *paquete = esperar_respuesta_cpu(socket_cpu_dispatch);//espera la respuesta de cpu, siempre envian y mandan paquetes
		proceso->tiempo_cpu += get_tiempo_actual() - tiempo_inicio_cpu;//como es el calculo?
		pthread_mutex_lock(&mutex_exec);
		proceso_ejecutando = false;//ya no esta ejecutando el proceso
		pthread_mutex_unlock(&mutex_exec);

		proceso->pcb = deserializar_pcb(paquete);

		switch(paquete->codigo_operacion) {//de cpu recibo un protocolo o cop
			case DESALOJO_POR_IO:
				recibir_datos(socket_cpu_dispatch, &(proceso->tiempo_io), sizeof(uint32_t));//recibe por dispatch, el tiempo de io q estuvo en cpu supongo
				pthread_mutex_lock(&mutex_blocked);
				proceso->estado = BLOCKED;
				proceso->tiempo_inicio_bloqueo = get_tiempo_actual();//arranca en 0 o 1
				proceso->pcb->estimacion_rafaga = calcular_estimacion_rafaga(proceso->tiempo_cpu, proceso->pcb->estimacion_rafaga);//se calcula la estimacion, como la hace?
				proceso->tiempo_cpu = 0;
				queue_push(cola_blocked, proceso);//se mete el proceso a la cola de bloqueados
				pthread_mutex_unlock(&mutex_blocked);

				sem_post(&sem_blocked);//post que hay un proceso en blocked
				break;
			case DESALOJO_POR_EXIT:
				pthread_mutex_lock(&mutex_exit);
				queue_push(cola_exit, proceso);//se mete el proceso a cola exit
				pthread_mutex_unlock(&mutex_exit);

				sem_post(&sem_exit);//post al plani largo plazo
				break;
			case DESALOJO_POR_IRQ:
				pthread_mutex_lock(&mutex_ready);
				list_add(cola_ready, proceso);//se agrega a la cola ready directamente
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
		sem_wait(&sem_blocked);//espera el post de exec
		pthread_mutex_lock(&mutex_blocked);
		t_proceso *proceso = queue_pop(cola_blocked);//saca proceso de cola de bloqueados
		pthread_mutex_unlock(&mutex_blocked);
		log_info(kernel_logger, "PID[%d] ingresa a BLOCKED", proceso->pcb->id);

		uint32_t tiempo_bloqueado = get_tiempo_actual() - proceso->tiempo_inicio_bloqueo;//el tiempo bloqueado es el actual - desde el inicio de bloqueo

		if(tiempo_bloqueado > kernel_config->tiempo_maximo_bloqueado) {// > al config
			transicion_suspender(proceso);//se suspende el proceso a la cola susp-blocked
			io(proceso->tiempo_io);//ese tiempo por 1000

			sem_post(&sem_suspended_ready);//y post a susp-ready
		} else if(tiempo_bloqueado + proceso->tiempo_io > kernel_config->tiempo_maximo_bloqueado) {//pensar como es esto
			uint32_t tiempo_io_antes_suspender = kernel_config->tiempo_maximo_bloqueado - tiempo_bloqueado;
			io(tiempo_io_antes_suspender);
			transicion_suspender(proceso);
			io(diferencia_absoluta(tiempo_io_antes_suspender, proceso->tiempo_io));

			sem_post(&sem_suspended_ready);//y post a susp-ready
		} else {//directamente se pasa a ready
			io(proceso->tiempo_io);
			pthread_mutex_lock(&mutex_ready);
			list_add(cola_ready, proceso);//se agrega a la lista ready el proceso
			pthread_mutex_unlock(&mutex_ready);

			sem_post(&sem_ready);//y post a ready
		}

	}
}

void enviar_proceso_a_cpu(t_proceso *proceso, int socket_cpu_dispatch) {//se envia pcb a cpu
	t_paquete *paquete = serializar_pcb(proceso->pcb, PCB);
	enviar_paquete(paquete, socket_cpu_dispatch);//se envia el pcb del proceso al modulo CPU
	eliminar_paquete(paquete);
}

t_paquete *esperar_respuesta_cpu(int socket_cpu_dispatch) {
	return recibir_paquete(socket_cpu_dispatch);
}

void enviar_interrupcion_a_cpu(int socket_fd, t_protocolo protocolo) {
	enviar_datos(socket_fd, &protocolo, sizeof(t_protocolo));
}

t_proceso *siguiente_a_ejecutar(char *algoritmo) {//segun fifo o srt, selecciona el siguiente proceso a ejecutar
	t_proceso *proceso = NULL;

	bool menor_rafaga(void *p1, void *p2) {//recibe 2 procesos
		uint32_t estimacion_p1 = ((t_proceso *)p1)->pcb->estimacion_rafaga - ((t_proceso *)p1)->tiempo_cpu;
		uint32_t estimacion_p2 = ((t_proceso *)p2)->pcb->estimacion_rafaga - ((t_proceso *)p2)->tiempo_cpu;
		return estimacion_p1 <= estimacion_p2;//retorna un booleano si estimacion de p1 < p2
	}

	pthread_mutex_lock(&mutex_ready);
	if(string_equals_ignore_case(algoritmo, "SRT")) {
		list_sort(cola_ready, menor_rafaga);//ordena la lista ready segun la menor rafaga
	}
	else if(string_equals_ignore_case(algoritmo, "FIFO")) {//no hace nada
	}

	proceso = (t_proceso *)list_remove(cola_ready, 0);//saca el primer elemento de la lista ready
	pthread_mutex_unlock(&mutex_ready);

	return proceso;//devuelve un proceso
}

static void io(uint32_t tiempo) {
	usleep(tiempo * 1000);
}

static uint32_t diferencia_absoluta(uint32_t tiempo_1, uint32_t tiempo_2) {
	return tiempo_1 > tiempo_2 ? tiempo_1 - tiempo_2 : tiempo_2 - tiempo_1;
}

static double calcular_estimacion_rafaga(uint32_t tiempo_ejecucion, double tiempo_estimado) {//tiempo_ejecucion, tiempo en cola exec
	return kernel_config->alfa * tiempo_ejecucion + (1 - kernel_config->alfa) * tiempo_estimado;//tiempo estimado anterior
}

