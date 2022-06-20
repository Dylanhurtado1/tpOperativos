#ifndef SERIALIZADOR_H_
#define SERIALIZADOR_H_

#include <commons/log.h>
#include <commons/collections/list.h>
#include "protocolo.h"
#include "paquete.h"
#include "estructuras.h"

t_paquete *serializar_consola(t_consola *consola, t_protocolo protocolo);
t_consola *deserializar_consola(t_paquete *paquete);
t_paquete *serializar_pcb(t_pcb *proceso, t_protocolo protocolo);
t_pcb *deserializar_pcb(t_paquete *paquete);
t_paquete *serializar_traductor(t_traductor *traductor, t_protocolo protocolo);
t_traductor *deserializar_traductor(t_paquete *paquete);
t_paquete *serializar_tabla_de_acceso(t_tabla_acceso *tabla_acceso, t_protocolo protocolo);
t_tabla_acceso *deserializar_tabla_de_acceso(t_paquete *paquete);


#endif /* SERIALIZADOR_H_ */
