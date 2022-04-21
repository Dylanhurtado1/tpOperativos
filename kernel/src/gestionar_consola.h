#ifndef GESTIONAR_CONSOLA_H_
#define GESTIONAR_CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <sockets.h>
#include <servidor.h>
#include <protocolo.h>
#include <comunicacion.h>
#include <estructuras.h>

void procesar_datos_consola(t_cliente *datos_cliente);
void enviar_respuesta_a_consola(int socket_fd, t_protocolo protocolo);
t_list *deserealizar_instrucciones(t_list *datos);
uint32_t obtener_tamanio_consola(t_list *datos);

#endif /* GESTIONAR_CONSOLA_H_ */
