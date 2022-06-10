#ifndef SERIALIZADOR_H_
#define SERIALIZADOR_H_

#include <commons/log.h>
#include <commons/collections/list.h>
#include "protocolo.h"
#include "paquete.h"
#include "estructuras.h"

t_paquete *serializar_consola(t_list *instrucciones, uint32_t tamanio_consola, t_protocolo protocolo);
t_consola *deserializar_consola(t_paquete *paquete);
t_paquete *serializar_pcb(t_pcb *proceso, t_protocolo protocolo);
t_pcb *deserializar_pcb(t_paquete *paquete);
t_paquete *serializar_traductor(t_traductor *traductor, t_protocolo protocolo);
t_traductor *deserializar_traductor(t_paquete *paquete);
t_paquete *serializar_acceso_tabla(uint32_t direccion, uint32_t entrada, t_protocolo protocolo);
t_acceso_tabla *deserializar_acceso_tabla(t_paquete *paquete);


#endif /* SERIALIZADOR_H_ */
