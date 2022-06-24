#ifndef PAGINACION_H_
#define PAGINACION_H_

#include "memoria_global.h"
#include "memoria_principal.h"
#include "swap.h"

typedef struct {
	uint32_t pid;
	uint32_t marco;
	uint8_t presencia;
	uint8_t uso;
	uint8_t modificado;
	uint8_t numero_pagina;
} t_pagina_segundo_nivel;

typedef struct {
	uint32_t pid;
	uint32_t puntero_a_pagina;
} t_algoritmo;

uint32_t crear_tablas_de_paginacion(uint32_t pid);
uint32_t get_tabla_segundo_nivel(uint32_t tabla_primer_nivel, uint32_t entrada_tabla);
uint32_t get_marco_de_pagina(uint32_t tabla_segundo_nivel, uint32_t entrada_tabla);
void actualizar_pagina_modificada(uint32_t direccion_fisica);


#endif /* PAGINACION_H_ */
