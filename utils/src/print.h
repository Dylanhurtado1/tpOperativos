#ifndef PRINT_H_
#define PRINT_H_

#include <commons/log.h>
#include "estructuras.h"

void print_datos_consola(t_log *logger, t_consola *consola);
void print_instrucciones(t_log *logger, t_list *instrucciones);
void print_pcb(t_log *logger, t_pcb *pcb);
void print_traductor(t_log *logger, t_traductor *traductor);

#endif /* PRINT_H_ */
