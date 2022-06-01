#ifndef MMU_H_
#define MMU_H_

#include "cpu_global.h"

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
uint32_t obtener_tabla_segundo_nivel(int socket_memoria, uint32_t tabla_primer_nivel, uint32_t entrada_tabla_nivel_1);
uint32_t obtener_numero_de_marco(int socket_memoria, uint32_t tabla_segundo_nivel, uint32_t entrada_tabla_nivel_2);
uint32_t obtener_valor_de_memoria(int socket_memoria, uint32_t direccion_fisica);
void enviar_valor_a_memoria(int socket_memoria, uint32_t direccion_fisica, uint32_t valor);


#endif /* MMU_H_ */
