#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "consola_config.h"
#include "parser.h"
#include <protocolo.h>
#include <conexiones.h>
#include <comunicacion.h>
#include <serializador.h>
#include <commons/log.h>
#include <stdio.h>
#include <stdlib.h>

#define PATH_CONSOLA_CONFIG "/home/utnso/tp-2022-1c-lo-importante-es-aprobar/consola/consola.config"

void enviar_datos_a_kernel(int socket_kernel, t_list *instrucciones, uint32_t tam_consola);
t_protocolo esperar_respuesta_de_kernel(int socket_kernel);


#endif /* CONSOLA_H_ */
