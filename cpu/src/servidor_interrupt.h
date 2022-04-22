#ifndef SERVIDOR_INTERRUPT_H_
#define SERVIDOR_INTERRUPT_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <cliente.h>
#include <servidor.h>
#include <sockets.h>
#include <protocolo.h>
#include <errors.h>
#include <estructuras.h>

int iniciar_servidor_interrupt(char *ip, char *puerto);
void peticiones_interrupt(int *socket_fd);

#endif /* SERVIDOR_INTERRUPT_H_ */
