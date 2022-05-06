#ifndef PETICIONES_H_
#define PETICIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <commons/log.h>
#include <cliente.h>
#include <servidor.h>
#include <sockets.h>
#include <protocolo.h>
#include <estructuras.h>
#include <serializador.h>
#include "ciclo_instruccion.h"

void peticiones_dispatch(int *socket_fd);
void peticiones_interrupt(int *socket_fd);
void eliminar_pcb(t_pcb *pcb);

//instrucciones con acceso a MEMORIA
void exec_instruccion_READ (int dir_logica);
void exec_instruccion_WRITE (int dir_logica, int valor);
void exec_instruccion_COPY (int dir_logica_destino, int dir_logica_origen);
int traducir_direccion(int dir_logica);

#endif /* PETICIONES_H_ */
