#ifndef PAQUETE_H_
#define PAQUETE_H_

#include "buffer.h"
#include <commons/collections/list.h>

typedef struct {
	uint8_t codigo_operacion;
	t_buffer *buffer;
} t_paquete;

/**
 * @NAME: crear_paquete
 * @DESC: Crea un paquete con un codigo de operacion y buffer especifico.
 */
t_paquete *crear_paquete(uint8_t codigo_operacion, t_buffer *buffer);

/*
 * @NAME: agregar_a_paquete
 * @DESC: Se agrega dato a paquete.
 * Tambien se incluye, al payload, el tamanio
 */
void agregar_a_paquete(t_paquete *paquete, void *dato, uint32_t tamanio);

void eliminar_paquete(t_paquete *paquete);
t_buffer* serializar_paquete(t_paquete *paquete);
t_list *deserealizar_paquete(t_paquete *paquete);
char *deserealizar_mensaje(t_paquete *paquete);


#endif
