#ifndef CPU_CONFIG_H_
#define CPU_CONFIG_H_

#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>

typedef struct {
	int entradas_tlb;
	char *reemplazo_tlb;
	int retardo_noop;
	char *ip_memoria;
	char *puerto_memoria;
	char *ip_cpu;
	char *puerto_escucha_dispatch;
	char *puerto_escucha_interrupt;
} t_cpu_config;

t_cpu_config *cpu_leer_configuracion(char *path);
void cpu_eliminar_configuracion(t_cpu_config *config);

#endif /* CPU_CONFIG_H_ */
