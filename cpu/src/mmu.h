#ifndef MMU_H_
#define MMU_H_

#include "cpu_global.h"
#include "tlb.h"

typedef struct {
	uint32_t numero_pagina;
	uint32_t entrada_tabla_nivel_1;
	uint32_t entrada_tabla_nivel_2;
	uint32_t desplazamiento;
} t_traducciones;

uint32_t traducir_direccion_logica(uint32_t tabla_primer_nivel, uint32_t direccion_logica);
uint32_t leer_memoria(uint32_t direccion_fisica);
void escribir_memoria(uint32_t direccion_fisica, uint32_t valor);
t_traducciones parsear_direccion_logica(uint32_t direcion_logica);
uint32_t acceder_tablas_en_memoria(uint32_t direccion_tabla, uint32_t entrada_tabla, t_protocolo protocolo);


#endif /* MMU_H_ */
