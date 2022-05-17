#ifndef GESTIONAR_CONSOLA_H_
#define GESTIONAR_CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <sockets.h>
#include <servidor.h>
#include <protocolo.h>
#include <comunicacion.h>
#include <estructuras.h>
#include <serializador.h>
#include "planificador.h"
#include "print.h"

void procesar_datos_consola(t_cliente *datos_cliente);

#endif /* GESTIONAR_CONSOLA_H_ */
