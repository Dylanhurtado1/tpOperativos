#ifndef GESTIONAR_CONEXIONES_H_
#define GESTIONAR_CONEXIONES_H_

#include "memoria_global.h"
#include "paginacion.h"

void procesar_conexiones(t_cliente *datos_cliente);
void enviar_direccion_tabla_de_pagina(int socket_fd, uint32_t direccion);
void enviar_numero_marco_de_pagina(int socket_fd, uint32_t numero);
void enviar_valor_leido_de_memoria(int socket_fd, uint32_t valor);
void enviar_estructura_traductora(int socket_fd, t_traductor *traductor);
t_traductor *crear_traductor(int entradas_tabla, int tamanio_pagina);
void eliminar_traductor(t_traductor *traductor);
void informar_estado_proceso(int socket_fd, t_protocolo protocolo);
void eliminar_pcb(t_pcb *pcb);
void eliminar_tabla_de_acceso(t_tabla_acceso *acceso_tabla);


#endif /* GESTIONAR_CONEXIONES_H_ */
