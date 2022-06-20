#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <protocolo.h>
#include <conexiones.h>
#include <comunicacion.h>
#include <serializador.h>
#include <estructuras.h>
#include "consola_config.h"
#include "parser.h"

#define PATH_CONSOLA_CONFIG "/home/utnso/tp-2022-1c-lo-importante-es-aprobar/consola/consola.config"

void enviar_consola_a_kernel(int socket_kernel, t_consola *consola);
t_protocolo esperar_respuesta_de_kernel(int socket_kernel);


#endif /* CONSOLA_H_ */
