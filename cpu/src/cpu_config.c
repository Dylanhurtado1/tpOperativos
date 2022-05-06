#include "cpu_config.h"

t_cpu_config *cpu_leer_configuracion(char *path) {

	t_config *config = config_create(path);
	t_cpu_config *consola_config = malloc(sizeof(t_cpu_config));

	consola_config->entradas_tlb = config_get_int_value(config, "ENTRADAS_TLB");
	consola_config->reemplazo_tlb = string_duplicate(config_get_string_value(config, "REEMPLAZO_TLB"));
	consola_config->retardo_noop = config_get_int_value(config, "RETARDO_NOOP");
	consola_config->ip_memoria = string_duplicate(config_get_string_value(config, "IP_MEMORIA"));
	consola_config->puerto_memoria = string_duplicate(config_get_string_value(config, "PUERTO_MEMORIA"));
	consola_config->ip_cpu = string_duplicate(config_get_string_value(config, "IP_CPU"));
	consola_config->puerto_escucha_dispatch = string_duplicate(config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH"));
	consola_config->puerto_escucha_interrupt = string_duplicate(config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT"));

	config_destroy(config);

	return consola_config;
}

void cpu_eliminar_configuracion(t_cpu_config *config) {
	free(config->reemplazo_tlb);
	free(config->ip_memoria);
	free(config->puerto_memoria);
	free(config->ip_cpu);
	free(config->puerto_escucha_dispatch);
	free(config->puerto_escucha_interrupt);
	free(config);
}
