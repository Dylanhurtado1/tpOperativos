#ifndef PRINT_H_
#define PRINT_H_

#include <stdint.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "estructuras.h"

void print_datos_consola(t_log *logger, t_list *instrucciones, uint32_t tamanio);
void print_instrucciones(t_log *logger, t_list *instrucciones);

#endif /* PRINT_H_ */
