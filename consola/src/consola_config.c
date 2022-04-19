/*
 * consola_config.c
 *
 *  Created on: 19 abr. 2022
 *      Author: utnso
 */

#include "consola_config.h"

t_consola_config *consola_leer_configuracion(char *path) {

	t_config *config = config_create(path);
	t_consola_config *consola_config = malloc(sizeof(t_consola_config));

	consola_config->ip = string_duplicate(config_get_string_value(config, "IP_KERNEL"));
	consola_config->puerto = string_duplicate(config_get_string_value(config, "PUERTO_KERNEL"));

	config_destroy(config);

	return consola_config;
}

void consola_eliminar_configuracion(t_consola_config *config) {
	free(config->ip);
	free(config->puerto);
	free(config);
}
