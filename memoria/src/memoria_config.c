#include "memoria_config.h"


t_memoria_config *memoria_leer_configuracion(char *path) {

	t_config *config = config_create(path);
	t_memoria_config *consola_config = malloc(sizeof(t_memoria_config));

	consola_config->ip_memoria = string_duplicate(config_get_string_value(config, "IP_MEMORIA"));
	consola_config->puerto_escucha = string_duplicate(config_get_string_value(config, "PUERTO_ESCUCHA"));
	consola_config->tamanio_memoria = config_get_int_value(config, "TAM_MEMORIA");
	consola_config->tamanio_pagina = config_get_int_value(config, "TAM_PAGINA");
	consola_config->entradas_por_tabla = config_get_int_value(config, "ENTRADAS_POR_TABLA");
	consola_config->retardo_memoria = config_get_int_value(config, "RETARDO_MEMORIA");
	consola_config->algoritmo_reemplazo = string_duplicate(config_get_string_value(config, "ALGORITMO_REEMPLAZO"));
	consola_config->marcos_por_proceso = config_get_int_value(config, "MARCOS_POR_PROCESO");
	consola_config->retardo_swap = config_get_int_value(config, "RETARDO_SWAP");
	consola_config->path_swap = string_duplicate(config_get_string_value(config, "PATH_SWAP"));

	config_destroy(config);

	return consola_config;
}

void memoria_eliminar_configuracion(t_memoria_config *config) {
	free(config->ip_memoria);
	free(config->puerto_escucha);
	free(config->algoritmo_reemplazo);
	free(config->path_swap);
	free(config);
}
