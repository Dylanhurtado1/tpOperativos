#ifndef SERIALIZADOR_H_
#define SERIALIZADOR_H_

#include <commons/log.h>
#include "errors.h"
#include "protocolo.h"
#include "paquete.h"
#include "estructuras.h"

t_paquete *serealizar_instrucciones(t_list *instrucciones, t_protocolo protocolo);
t_traductor *deserealizar_traductor(t_paquete *paquete);

#endif /* SERIALIZADOR_H_ */
