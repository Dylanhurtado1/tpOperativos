#include "conexiones.h"
#include <unistd.h>

int conectar_a_modulo(char *ip, char *puerto, t_log *logger) {
	int timeout = 20;
	int socket_server;

	while((socket_server = conectar_a_servidor(ip, puerto)) == SERVER_CONNECTION_ERROR && timeout > 0) {
		log_error(logger, "Esperando a servidor");
		sleep(1);
		timeout--;
	}

	if(timeout <= 0) {
		log_error(logger, "Error al conectar con servidor (timeout): IP = %s, PUERTO = %s", ip, puerto);
		log_destroy(logger);
		abort();
	}
	return socket_server;
}

int iniciar_modulo_servidor(char *ip, char *puerto, t_log *logger) {
	int socket_server = crear_servidor(ip, puerto);
	if(socket_server == INIT_SERVER_ERROR) {
		log_error(logger, "No se pudo iniciar el servidor: IP = %s, PUERTO = %s", ip, puerto);
		log_destroy(logger);
		abort();
	}
	return socket_server;
}
