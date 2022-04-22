#include "servidor_dispatch.h"

extern t_log *cpu_logger;

int iniciar_servidor_dispatch(char *ip, char *puerto) {

	int socket_dispatch = crear_servidor(ip, puerto);
	if(socket_dispatch == INIT_SERVER_ERROR) {
		log_error(cpu_logger, "No se pudo iniciar el servidor de Dispatch");
		return EXIT_FAILURE;
	}
	return socket_dispatch;
}

void peticiones_dispatch(int *socket_dispatch) {
	log_info(cpu_logger, "CPU escuchando puerto dispatch");
	int socket_kernel = esperar_cliente(*socket_dispatch);
	while (true) {
		t_paquete *paquete = recibir_paquete(socket_kernel);
		switch (paquete->codigo_operacion) {
			case PCB:
				log_info(cpu_logger,"PCB recibida");
				break;
			default:
				log_error(cpu_logger,"Operacion desconocida.");
				break;
		}
		eliminar_paquete(paquete);
	}
}
