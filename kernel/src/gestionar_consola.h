#ifndef GESTIONAR_CONSOLA_H_
#define GESTIONAR_CONSOLA_H_

#include <protocolo.h>
#include <servidor.h>
#include <comunicacion.h>
#include <estructuras.h>
#include <serializador.h>
#include <print.h>
#include "planificador.h"
#include "kernel_global.h"

void procesar_datos_consola(t_cliente *datos_cliente);

#endif /* GESTIONAR_CONSOLA_H_ */
