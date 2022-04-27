#include "kernel.h"

t_log *kernel_logger;
t_kernel_config *kernel_config;

int socket_memoria;
int socket_cpu_dispatch;
int socket_cpu_interrupt;


int main(void) {

	iniciar_colas_de_planificacion();


	kernel_logger = log_create("kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
	kernel_config = kernel_leer_configuracion(PATH_KERNEL_CONFIG);

	socket_memoria = conectar_a_modulo(kernel_config->ip_memoria, kernel_config->puerto_memoria, kernel_logger);

	socket_cpu_dispatch = conectar_a_modulo(kernel_config->ip_cpu, kernel_config->puerto_cpu_dispatch, kernel_logger);

	socket_cpu_interrupt = conectar_a_modulo(kernel_config->ip_cpu, kernel_config->puerto_cpu_interrupt, kernel_logger);


	int socket_kernel = iniciar_modulo_servidor(kernel_config->ip_kernel, kernel_config->puerto_escucha, kernel_logger);

	log_info(kernel_logger, "Iniciando planificacion");


	if(strcmp(kernel_config->algoritmo_planificacion,"FIFO") == 0){
	log_info(kernel_logger,"Algoritmo de planificacion leido: FIFO");
	pthread_t hilo_planificacion_FIFO;
	pthread_create(&hilo_planificacion_FIFO, NULL, (void*) planificacionFIFO, NULL);
	pthread_detach(hilo_planificacion_FIFO);

	}

	else {

	log_info(kernel_logger,"Algoritmo de planificacion leido: SRT");
	pthread_t hilo_planificacion_SRT;
	pthread_create(&hilo_planificacion_SRT, NULL, (void*) planificacionSRT, NULL);
	pthread_detach(hilo_planificacion_SRT);

	}


	log_info(kernel_logger, "Esperando conexion de consolas");
	if(atender_clientes(socket_kernel, procesar_datos_consola) == WAIT_CLIENT_ERROR) {
		log_error(kernel_logger, "Error al escuchar clientes... Finalizando servidor");
		}
	log_destroy(kernel_logger);
	kernel_eliminar_configuracion(kernel_config);
	cerrar_conexion(socket_kernel);

	return EXIT_SUCCESS;




}

void iniciar_colas_de_planificacion() {
	iniciar_cola_new();
	iniciar_cola_ready();
	iniciar_cola_blocked();
	iniciar_cola_suspended_blocked();
	iniciar_cola_suspended_ready();
}
