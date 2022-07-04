#ifndef GESTIONAR_CONEXIONES_H_
#define GESTIONAR_CONEXIONES_H_

#include "memoria_global.h"
#include "memoria_principal.h"
#include "paginacion.h"
#include "swap.h"

void procesar_conexiones(t_cliente *datos_cliente);
void enviar_direccion_tabla_primer_nivel(int socket_fd, uint32_t direccion);
void informar_estado_proceso(int socket_fd, t_protocolo protocolo);
void enviar_estructura_traductora(int socket_fd, t_traductor *traductor);
void enviar_direccion_tabla_segundo_nivel(int socket_fd, uint32_t direccion);
void enviar_numero_marco_de_pagina(int socket_fd, uint32_t numero);
void enviar_valor_leido_de_memoria(int socket_fd, uint32_t valor);


#endif /* GESTIONAR_CONEXIONES_H_ */
