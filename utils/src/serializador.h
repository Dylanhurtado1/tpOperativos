#ifndef SERIALIZADOR_H_
#define SERIALIZADOR_H_

#include <commons/log.h>
#include "errors.h"
#include "protocolo.h"
#include "paquete.h"
#include "estructuras.h"

t_paquete *serializar_instrucciones(t_list *instrucciones, t_protocolo protocolo);
t_list *deserializar_instrucciones(t_list *datos, t_log *logger);
t_paquete *serializar_pcb(t_pcb *proceso, t_protocolo protocolo);
t_pcb *deserializar_pcb(t_paquete *paquete, t_log *logger);
t_paquete *serializar_traductor(t_traductor *traductor, t_protocolo protocolo);
t_traductor *deserializar_traductor(t_paquete *paquete, t_log *logger);

#endif /* SERIALIZADOR_H_ */
