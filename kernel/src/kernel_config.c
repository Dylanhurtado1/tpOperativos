#include "kernel_config.h"

t_kernel_config *kernel_leer_configuracion(char *path) {

	t_config *config = config_create(path);
	t_kernel_config *consola_config = malloc(sizeof(t_kernel_config));

	consola_config->ip_memoria = string_duplicate(config_get_string_value(config, "IP_MEMORIA"));
	consola_config->puerto_memoria = string_duplicate(config_get_string_value(config, "PUERTO_MEMORIA"));
	consola_config->ip_cpu = string_duplicate(config_get_string_value(config, "IP_CPU"));
	consola_config->puerto_cpu_dispatch = string_duplicate(config_get_string_value(config, "PUERTO_CPU_DISPATCH"));
	consola_config->puerto_cpu_interrupt = string_duplicate(config_get_string_value(config, "PUERTO_CPU_INTERRUPT"));
	consola_config->ip_kernel = string_duplicate(config_get_string_value(config, "IP_KERNEL"));
	consola_config->puerto_escucha = string_duplicate(config_get_string_value(config, "PUERTO_ESCUCHA"));
	consola_config->algoritmo_planificacion = string_duplicate(config_get_string_value(config, "ALGORITMO_PLANIFICACION"));
	consola_config->estimacion_inicial = config_get_int_value(config, "ESTIMACION_INICIAL");
	consola_config->alfa = config_get_double_value(config, "ALFA");
	consola_config->grado_multiprogramacion = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
	consola_config->tiempo_maximo_bloqueado = config_get_int_value(config, "TIEMPO_MAXIMO_BLOQUEADO");

	config_destroy(config);

	return consola_config;
}

void kernel_eliminar_configuracion(t_kernel_config *config) {
	free(config->ip_memoria);
	free(config->puerto_memoria);
	free(config->ip_cpu);
	free(config->puerto_cpu_dispatch);
	free(config->puerto_cpu_interrupt);
	free(config->ip_kernel);
	free(config->puerto_escucha);
	free(config->algoritmo_planificacion);
	free(config);
}
