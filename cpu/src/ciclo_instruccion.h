#ifndef CICLO_INSTRUCCION_H_
#define CICLO_INSTRUCCION_H_

#include "cpu_global.h"
#include "mmu.h"

typedef enum {
	DESALOJO_IO,
	DESALOJO_EXIT,
	DESALOJO_INTERRUPCION,
	NO_DESALOJAR
} t_desalojo;

void ejecutar_ciclo_de_instruccion(t_pcb *pcb, int socket_kernel);
t_instruccion *fetch(t_pcb *pcb);
bool decode(t_instruccion *proxima_instruccion);
uint32_t fetch_operands(uint32_t tabla_primer_nivel, uint32_t direccion_logica);
t_desalojo execute(t_instruccion *instruccion, uint32_t tabla_primer_nivel);
bool check_interrupt();
void desalojar_proceso(t_pcb *pcb, t_desalojo status, int socket_kernel);


#endif /* CICLO_INSTRUCCION_H_ */
