/*
 * consola_config.h
 *
 *  Created on: 19 abr. 2022
 *      Author: utnso
 */

#ifndef MEMORIA_CONFIG_H_
#define MEMORIA_CONFIG_H_

#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>

typedef struct {
	char *ip_memoria;
	char *puerto_escucha;
	int tamanio_memoria;
	int tamanio_pagina;
	int entradas_por_tabla;
	int retardo_memoria;
	char *algoritmo_reemplazo;
	int marcos_por_proceso;
	int retardo_swap;
	char *path_swap;
} t_memoria_config;

t_memoria_config *memoria_leer_configuracion(char *path);
void memoria_eliminar_configuracion(t_memoria_config *config);

#endif /* MEMORIA_CONFIG_H_ */
