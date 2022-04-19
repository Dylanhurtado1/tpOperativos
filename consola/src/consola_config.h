/*
 * consola_config.h
 *
 *  Created on: 19 abr. 2022
 *      Author: utnso
 */

#ifndef CONSOLA_CONFIG_H_
#define CONSOLA_CONFIG_H_

#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>

typedef struct {
	char *ip;
	char *puerto;
} t_consola_config;

t_consola_config *consola_leer_configuracion(char *path);
void consola_eliminar_configuracion(t_consola_config *config);

#endif /* CONSOLA_CONFIG_H_ */
