#ifndef PAGINACION_H_
#define PAGINACION_H_

#include "memoria_global.h"

typedef struct {
	uint32_t pid;
	uint32_t marco;
	uint8_t presencia;
	uint8_t uso;
	uint8_t modificado;
} t_pagina_segundo_nivel;

typedef struct {
	uint32_t pid;
	uint8_t libre;
	uint32_t numero;
} t_marco;

uint32_t crear_tablas_de_paginacion(t_pcb *pcb);
uint32_t get_tabla_segundo_nivel(uint32_t tabla_primer_nivel, uint32_t entrada_tabla);
uint32_t get_marco_de_pagina(uint32_t tabla_segundo_nivel, uint32_t entrada_tabla);

void iniciar_marcos_memoria();
bool page_fault(t_pagina_segundo_nivel *pagina);
uint32_t cantidad_marcos_asignados(uint32_t pid);
void asignar_marco_libre(t_pagina_segundo_nivel *pagina);
void reemplazar_marco(t_pagina_segundo_nivel *pagina_a_agregar, char *algoritmo_reemplazo);


#endif /* PAGINACION_H_ */
