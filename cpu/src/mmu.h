#ifndef MMU_H_
#define MMU_H_

#include "cpu_global.h"

typedef struct {
	uint32_t numero_pagina;
	uint32_t entrada_tabla_nivel_1;
	uint32_t entrada_tabla_nivel_2;
	uint32_t desplazamiento;
} t_traducciones;

uint32_t mmu_leer_memoria(uint32_t tabla_primer_nivel, uint32_t direccion_logica);
t_traducciones get_traducciones(uint32_t direcion_logica);
uint32_t obtener_tabla_segundo_nivel(int socket_memoria, uint32_t tabla_primer_nivel, uint32_t entrada_tabla_nivel_1);
uint32_t obtener_numero_de_marco(int socket_memoria, uint32_t tabla_segundo_nivel, uint32_t entrada_tabla_nivel_2);
uint32_t obtener_valor_en_memoria(int socket_memoria, uint32_t direccion_fisica);

void exec_instruccion_READ (int dir_logica);
void exec_instruccion_WRITE (int dir_logica, int valor);
void exec_instruccion_COPY (int dir_logica_destino, int dir_logica_origen);

#endif /* MMU_H_ */
