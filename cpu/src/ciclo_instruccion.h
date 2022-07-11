#ifndef CICLO_INSTRUCCION_H_
#define CICLO_INSTRUCCION_H_

#include "cpu_global.h"
#include "mmu.h"

typedef enum {
	CONTINUAR_EJECUTANDO,
	PROCESO_DESALOJADO
} t_desalojo;//tiene 2 atributos, para volver a hacer el ciclo de instruccion o hacer desalojo enviando a kernel

void ejecutar_ciclo_de_instruccion(t_pcb *pcb, int socket_kernel);
t_instruccion *fetch(t_pcb *pcb);
bool decode(t_instruccion *proxima_instruccion);
uint32_t fetch_operands(uint32_t tabla_primer_nivel, uint32_t direccion_logica);
t_desalojo execute(t_instruccion *instruccion, t_pcb *pcb, int socket_kernel);
bool check_interrupt();
void desalojar_proceso(int socket_kernel, t_pcb *pcb, t_protocolo protocolo);
void enviar_tiempo_io(int socket_kernel, uint32_t tiempo);


#endif /* CICLO_INSTRUCCION_H_ */
