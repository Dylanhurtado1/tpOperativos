#ifndef MMU_H_
#define MMU_H_

#include "cpu_global.h"

uint32_t leer_de_memoria(uint32_t tabla_primer_nivel, uint32_t direccion_logica);
uint32_t traducir_direccion(uint32_t direcion_logica);
void exec_instruccion_READ (int dir_logica);
void exec_instruccion_WRITE (int dir_logica, int valor);
void exec_instruccion_COPY (int dir_logica_destino, int dir_logica_origen);

#endif /* MMU_H_ */
