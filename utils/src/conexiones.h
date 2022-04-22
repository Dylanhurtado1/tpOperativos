#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include "cliente.h"
#include "servidor.h"
#include "errors.h"

int conectar_a_modulo(char *ip, char *puerto, t_log *logger);
int iniciar_modulo_servidor(char *ip, char *puerto, t_log *logger);

#endif /* CONEXIONES_H_ */
