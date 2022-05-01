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

// funciones del ciclo de instruccion
void ejecutar_ciclo_de_instruccion(t_pcb *pcb);
t_instruccion *fetch(t_pcb *pcb);
bool decode(t_instruccion *proxima_instruccion);
void execute(t_instruccion *instruccion, uint32_t valor);
uint32_t fetch_operands(uint32_t operando, int socket_fd);

#endif /* PETICIONES_H_ */
