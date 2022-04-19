#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include "sockets.h"
#include "buffer.h"
#include "paquete.h"

void enviar_mensaje(char *mensaje, uint8_t codigo_operacion, int socket_conexion);
void enviar_paquete(t_paquete *paquete, int socket_conexion);
t_paquete *recibir_paquete(int socket_cliente);
uint8_t recibir_operacion(int socket_cliente);
t_buffer *recibir_buffer(int socket_cliente);


#endif
