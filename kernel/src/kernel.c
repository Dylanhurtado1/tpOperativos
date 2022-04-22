#include "kernel.h"

t_log *kernel_logger;
t_kernel_config *kernel_config;

int socket_memoria;
int socket_cpu_dispatch;
int socket_cpu_interrupt;

int main(void) {
	kernel_logger = log_create("kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
	kernel_config = kernel_leer_configuracion(PATH_KERNEL_CONFIG);
	iniciar_colas_de_planificacion();

	socket_memoria = conectar_a_memoria(kernel_config->ip_memoria, kernel_config->puerto_memoria);
	if(socket_memoria == SERVER_CONNECTION_ERROR) {
		log_error(kernel_logger, "Error al conectar con Memoria");
		log_destroy(kernel_logger);
		return EXIT_FAILURE;
	}

	socket_cpu_dispatch = conectar_a_cpu(kernel_config->ip_cpu, kernel_config->puerto_cpu_dispatch);
	if(socket_cpu_dispatch == SERVER_CONNECTION_ERROR) {
		log_error(kernel_logger, "Error al conectar con CPU Dispatch");
		log_destroy(kernel_logger);
		return EXIT_FAILURE;
	}

	socket_cpu_interrupt = conectar_a_cpu(kernel_config->ip_cpu, kernel_config->puerto_cpu_interrupt);
	if(socket_cpu_interrupt == SERVER_CONNECTION_ERROR) {
		log_error(kernel_logger, "Error al conectar con CPU Interrupt");
		log_destroy(kernel_logger);
		return EXIT_FAILURE;
	}

	int socket_kernel = crear_servidor(kernel_config->ip_kernel, kernel_config->puerto_escucha);
	if(socket_kernel == INIT_SERVER_ERROR) {
		log_error(kernel_logger, "No se pudo iniciar el servidor Kernel");
		return EXIT_FAILURE;
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

int conectar_a_memoria(char *ip, char *puerto) {
	return conectar_a_servidor(ip, puerto);
}

int conectar_a_cpu(char *ip, char *puerto) {
	return conectar_a_servidor(ip, puerto);
}

void iniciar_colas_de_planificacion() {
	iniciar_cola_new();
	iniciar_cola_ready();
}
