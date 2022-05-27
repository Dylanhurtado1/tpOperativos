#include "consola_config.h"


t_consola_config *consola_leer_configuracion(char *path) {

	t_config *config = config_create(path);
	t_consola_config *consola_config = malloc(sizeof(t_consola_config));

	consola_config->ip_kernel = string_duplicate(config_get_string_value(config, "IP_KERNEL"));
	consola_config->puerto_kernel = string_duplicate(config_get_string_value(config, "PUERTO_KERNEL"));

	config_destroy(config);

	return consola_config;
}

void consola_eliminar_configuracion(t_consola_config *config) {
	free(config->ip_kernel);
	free(config->puerto_kernel);
	free(config);
}
