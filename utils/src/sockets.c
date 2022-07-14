#include "sockets.h"
#include "errors.h"

int iniciar_servidor(char *ip, char *puerto) {
	int socket_servidor;
    struct addrinfo hints, *server_info, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &server_info);

    for (p = server_info; p != NULL; p = p->ai_next) {
        socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socket_servidor == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

    if(p == NULL)
    	return SOCKET_CREATION_ERROR;

	if(listen(socket_servidor, SOMAXCONN) == -1)
		return LISTEN_ERROR;

    const int enable = 1;
    if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");
    if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEPORT) failed");

    freeaddrinfo(server_info);

    return socket_servidor;
}

int esperar_cliente(int socket_servidor) {
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (struct sockaddr *) &dir_cliente, &tam_direccion);
	if(socket_cliente == -1)
		return ACCEPT_ERROR;

	return socket_cliente;
}

int crear_conexion(char *ip, char* puerto) {
	struct addrinfo hints, *server_info;

	memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int conexion = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	if (conexion == -1) {
		return SOCKET_CREATION_ERROR;
	}

	if(connect(conexion, server_info->ai_addr, server_info->ai_addrlen) == -1) {
		freeaddrinfo(server_info);
		return CONNECT_ERROR;
	}

	freeaddrinfo(server_info);

	return conexion;
}

void cerrar_conexion(int socket_fd) {
	close(socket_fd);
}


int enviar_datos(int socket_fd, void *source, uint32_t size) {
	return send(socket_fd, source, size, 0);
}

int recibir_datos(int socket_fd, void *dest, uint32_t size) {
	return recv(socket_fd, dest, size, MSG_WAITALL);
}
