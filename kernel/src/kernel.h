/*
 * kernel.h
 *
 *  Created on: 19 abr. 2022
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <protocolo.h>
#include <conexiones.h>
#include "kernel_config.h"
#include "gestionar_consola.h"
#include "planificador.h"

#define PATH_KERNEL_CONFIG "/home/utnso/tp-2022-1c-lo-importante-es-aprobar/kernel/kernel.config"

void iniciar_colas_de_planificacion();

#endif /* KERNEL_H_ */
