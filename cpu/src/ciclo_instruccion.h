#ifndef CICLO_INSTRUCCION_H_
#define CICLO_INSTRUCCION_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <commons/log.h>
#include <cliente.h>
#include <servidor.h>
#include <sockets.h>
#include <protocolo.h>
#include <estructuras.h>
#include <serializador.h>
#include "cpu_config.h"

void ejecutar_ciclo_de_instruccion(t_pcb *pcb, int socket_kernel);
t_instruccion *fetch(t_pcb *pcb);
bool decode(t_instruccion *proxima_instruccion);
uint32_t fetch_operands(uint32_t direccion_logica, int socket_memoria);
int execute(t_instruccion *instruccion, uint32_t valor);
bool check_interrupt();

#endif /* CICLO_INSTRUCCION_H_ */
