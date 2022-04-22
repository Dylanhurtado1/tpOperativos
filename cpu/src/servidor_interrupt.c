#include "servidor_interrupt.h"

extern t_log *cpu_logger;

int iniciar_servidor_interrupt(char *ip, char *puerto) {

	int socket_interrupt = crear_servidor(ip, puerto);
	if(socket_interrupt == INIT_SERVER_ERROR) {
		log_error(cpu_logger, "No se pudo iniciar el servidor de Interrupt");
		return EXIT_FAILURE;
	}

	return socket_interrupt;
}

void peticiones_interrupt(int *socket_interrupt) {
	log_info(cpu_logger, "CPU escuchando puerto interrupt");
	int socket_kernel = esperar_cliente(*socket_interrupt);
	while (true) {
		t_paquete *paquete = recibir_paquete(socket_kernel);
		switch (paquete->codigo_operacion) {
			default:
				log_error(cpu_logger,"Operacion desconocida.");
				break;
		}
		eliminar_paquete(paquete);
	}
}
