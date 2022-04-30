#ifndef PETICIONES_H_
#define PETICIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <cliente.h>
#include <servidor.h>
#include <sockets.h>
#include <protocolo.h>
#include <errors.h>
#include <estructuras.h>

void peticiones_dispatch(int *socket_fd);
void peticiones_interrupt(int *socket_fd);
t_pcb *deserealizar_pcb(t_paquete *paquete);
void eliminar_pcb(t_pcb *pcb);

#endif /* PETICIONES_H_ */
