#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <conexiones.h>
#include <errors.h>
#include <comunicacion.h>
#include <protocolo.h>
#include "consola_config.h"
#include "parser.h"

#define PATH_CONSOLA_CONFIG "/home/utnso/tp-2022-1c-lo-importante-es-aprobar/consola/consola.config"

void enviar_datos_a_kernel(int socket_kernel, t_list *instrucciones, uint32_t tam_consola);
t_protocolo esperar_respuesta_de_kernel(int socket_kernel);
t_paquete *serializar_datos_consola(t_list *instrucciones, uint32_t tam_consola);

#endif /* CONSOLA_H_ */
