#ifndef MMU_H_
#define MMU_H_

#include "cpu_global.h"
#include "tlb.h"

typedef struct {
	uint32_t numero_pagina;
	uint32_t entrada_tabla_nivel_1;
	uint32_t entrada_tabla_nivel_2;
	uint32_t desplazamiento;
} t_traducciones;//la traduccion tiene el formato de la dir logica

uint32_t mmu_traducir_direccion_logica(uint32_t tabla_primer_nivel, uint32_t direccion_logica);
uint32_t mmu_leer_memoria(uint32_t direccion_fisica);
void mmu_escribir_memoria(uint32_t direccion_fisica, uint32_t valor);


#endif /* MMU_H_ */
