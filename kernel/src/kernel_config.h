#ifndef KERNEL_CONFIG_H_
#define KERNEL_CONFIG_H_

#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>

typedef struct {
	char *ip_memoria;
	char *puerto_memoria;
	char *ip_cpu;
	char *puerto_cpu_dispatch;
	char *puerto_cpu_interrupt;
	char *ip_kernel;
	char *puerto_escucha;
	char *algoritmo_planificacion;
	int estimacion_inicial;
	double alfa;
	int grado_multiprogramacion;
	int tiempo_maximo_bloqueado;
} t_kernel_config;

t_kernel_config *kernel_leer_configuracion(char *path);
void kernel_eliminar_configuracion(t_kernel_config *config);

#endif /* KERNEL_CONFIG_H_ */
