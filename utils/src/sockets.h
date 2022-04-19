#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int iniciar_servidor(char *ip, char *puerto);
int esperar_cliente(int socket_servidor);
int crear_conexion(char *ip, char *puerto);
void cerrar_conexion(int socket_fd);
int enviar_datos(int socket_fd, void *source, uint32_t size);
int recibir_datos(int socket_fd, void *dest, uint32_t size);

#endif
