#ifndef PETICIONES_H_
#define PETICIONES_H_

#include "cpu_global.h"
#include "ciclo_instruccion.h"

void peticiones_dispatch(int *socket_fd);
void peticiones_interrupt(int *socket_fd);


#endif /* PETICIONES_H_ */
