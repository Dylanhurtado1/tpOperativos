#include "kernel.h"

t_log *kernel_logger;

int main(void) {
	kernel_logger = log_create("kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
	t_kernel_config *config = kernel_leer_configuracion(PATH_KERNEL_CONFIG);

	int socker_kernel = crear_servidor(config->ip_kernel, config->puerto_escucha);
	if(socker_kernel == INIT_SERVER_ERROR) {
		log_error(kernel_logger, "No se pudo iniciar el servidor Kernel");
		return EXIT_FAILURE;
	}

	if(atender_clientes(socker_kernel, procesar_datos_consola) == WAIT_CLIENT_ERROR) {
		log_error(kernel_logger, "Error al escuchar clientes... Finalizando servidor");
	}

	log_destroy(kernel_logger);
	kernel_eliminar_configuracion(config);
	cerrar_conexion(socker_kernel);

	return EXIT_SUCCESS;
}


