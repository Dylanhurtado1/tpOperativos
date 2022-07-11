#include "kernel.h"


int main(void) {

	kernel_logger = log_create("kernel.log", "KERNEL", true, LOG_LEVEL_INFO);//se crea un logger global
	kernel_config = kernel_leer_configuracion(PATH_KERNEL_CONFIG);//un config global de tipo kernel

	socket_memoria = conectar_a_modulo(kernel_config->ip_memoria, kernel_config->puerto_memoria, kernel_logger);//se conecta con memoria como cliente

	socket_cpu_dispatch = conectar_a_modulo(kernel_config->ip_cpu, kernel_config->puerto_cpu_dispatch, kernel_logger);//se conecta a cpu por socket dispatch

	socket_cpu_interrupt = conectar_a_modulo(kernel_config->ip_cpu, kernel_config->puerto_cpu_interrupt, kernel_logger);//se conecta a cpu por socket interrupt

	int socket_kernel = iniciar_modulo_servidor(kernel_config->ip_kernel, kernel_config->puerto_escucha, kernel_logger);//kernel es servidor de consola

	iniciar_planificador();//inicio de planificadores

	log_info(kernel_logger, "Esperando conexion de consolas");
	if(atender_clientes(socket_kernel, procesar_datos_consola) == WAIT_CLIENT_ERROR) {//while(1) escuchando continuamente
		log_error(kernel_logger, "Error al escuchar clientes... Finalizando servidor");
	}
	log_destroy(kernel_logger);
	kernel_eliminar_configuracion(kernel_config);
	cerrar_conexion(socket_kernel);

	return EXIT_SUCCESS;

}

void iniciar_planificador() {
	iniciar_planificador_largo_plazo();//se crean colas new y exit, e inicio de semaforos
	iniciar_planificador_corto_plazo();//colas ready,exec, y blocked
	iniciar_planificador_mediano_plazo();//cola suspendidos
}


