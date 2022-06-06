#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "memoria_global.h"

#define PATH_MEMORIA_CONFIG "/home/utnso/tp-2022-1c-lo-importante-es-aprobar/memoria/memoria.config"

void* memoria_principal;
pthread_mutex_t MUTEX_MP;

void procesar_conexiones(t_cliente *datos_cliente);
void enviar_numero_tabla_de_pagina(int socket_fd, uint32_t numero);
void enviar_estructura_traductora(int socket_fd, t_traductor *traductor);
t_traductor *crear_traductor(int entradas_tabla, int tamanio_pagina);
void eliminar_traductor(t_traductor *traductor);
void informar_proceso_suspendido(int socket_fd, t_protocolo protocolo);
void eliminar_pcb(t_pcb *pcb);


#endif /* MEMORIA_H_ */
