#include "cpu_config.h"


t_cpu_config *cpu_leer_configuracion(char *path) {

	t_config *config = config_create(path);
	t_cpu_config *cpu_config = malloc(sizeof(t_cpu_config));

	cpu_config->entradas_tlb = config_get_int_value(config, "ENTRADAS_TLB");
	cpu_config->reemplazo_tlb = string_duplicate(config_get_string_value(config, "REEMPLAZO_TLB"));
	cpu_config->retardo_noop = config_get_int_value(config, "RETARDO_NOOP");
	cpu_config->ip_memoria = string_duplicate(config_get_string_value(config, "IP_MEMORIA"));
	cpu_config->puerto_memoria = string_duplicate(config_get_string_value(config, "PUERTO_MEMORIA"));
	cpu_config->ip_cpu = string_duplicate(config_get_string_value(config, "IP_CPU"));
	cpu_config->puerto_escucha_dispatch = string_duplicate(config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH"));
	cpu_config->puerto_escucha_interrupt = string_duplicate(config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT"));

	config_destroy(config);

	return cpu_config;
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
