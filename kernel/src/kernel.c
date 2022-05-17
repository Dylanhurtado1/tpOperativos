#include "kernel.h"


int main(void) {

	kernel_logger = log_create("kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
	kernel_config = kernel_leer_configuracion(PATH_KERNEL_CONFIG);

	socket_memoria = conectar_a_modulo(kernel_config->ip_memoria, kernel_config->puerto_memoria, kernel_logger);

	socket_cpu_dispatch = conectar_a_modulo(kernel_config->ip_cpu, kernel_config->puerto_cpu_dispatch, kernel_logger);

	socket_cpu_interrupt = conectar_a_modulo(kernel_config->ip_cpu, kernel_config->puerto_cpu_interrupt, kernel_logger);

	int socket_kernel = iniciar_modulo_servidor(kernel_config->ip_kernel, kernel_config->puerto_escucha, kernel_logger);

	iniciar_planificador();

	log_info(kernel_logger, "Esperando conexion de consolas");
	if(atender_clientes(socket_kernel, procesar_datos_consola) == WAIT_CLIENT_ERROR) {
		log_error(kernel_logger, "Error al escuchar clientes... Finalizando servidor");
	}
	log_destroy(kernel_logger);
	kernel_eliminar_configuracion(kernel_config);
	cerrar_conexion(socket_kernel);

	return EXIT_SUCCESS;

}

void iniciar_planificador() {
	iniciar_planificador_largo_plazo();
	iniciar_planificador_corto_plazo();
}


