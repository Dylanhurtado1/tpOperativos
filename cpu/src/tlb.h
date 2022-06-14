#ifndef TLB_H_
#define TLB_H_

#include "cpu_global.h"

typedef struct {
	uint32_t pagina;
	uint32_t marco;
	uint32_t indice_usado;
} t_tlb;

bool tlb_hit(uint32_t pagina);
uint32_t tlb_marco_de_pagina(uint32_t pagina);
void tlb_agregar_entrada(uint32_t pagina, uint32_t marco);
void tlb_eliminar_entradas();


#endif /* TLB_H_ */
