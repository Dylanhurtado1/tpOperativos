#ifndef ERRORS_H_
#define ERRORS_H_

#include <stdbool.h>

typedef enum {
	SOCKET_CREATION_ERROR = 50,		// Error al crear el socket
	LISTEN_ERROR, 					// No se pudo habilitar al socket para aceptar conexiones
	CONNECT_ERROR, 					// Error al conectar al servidor
	ACCEPT_ERROR,					// Error al esperar un cliente
	RECEIVE_DATA_ERROR,				// Error al recibir datos del socket
	SEND_DATA_ERROR,				// Error al enviar datos al socket

	INIT_SERVER_ERROR,				// Error al iniciar el servidor
	WAIT_CLIENT_ERROR,				// Error atendiendo clientes
	SERVER_CONNECTION_ERROR			// Error en la conexion con el servidor
} t_socket_error;


bool es_codigo_de_error(t_socket_error codigo);

#endif /* ERRORS_H_ */
