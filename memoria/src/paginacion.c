#include "paginacion.h"

uint32_t indice_tabla_primer_nivel = 0;


uint32_t crear_tablas_de_paginacion(t_pcb *pcb) {
	uint32_t tabla_primer_nivel = indice_tabla_primer_nivel;
	uint32_t cantidad_entradas = memoria_config->entradas_por_tabla * memoria_config->entradas_por_tabla;

	for(int i = 0; i < cantidad_entradas; i++) {
		t_pagina_segundo_nivel *entrada_segundo_nivel = malloc(sizeof(t_pagina_segundo_nivel));
		entrada_segundo_nivel->pid = pcb->id;
		entrada_segundo_nivel->marco = 10;
		entrada_segundo_nivel->presencia = 0;
		entrada_segundo_nivel->usado = 0;
		entrada_segundo_nivel->modificado = 0;
		list_add(tablas_de_paginacion, entrada_segundo_nivel);

	}
	indice_tabla_primer_nivel += cantidad_entradas;

	return tabla_primer_nivel;
}

uint32_t get_tabla_segundo_nivel(uint32_t tabla_primer_nivel, uint32_t entrada_tabla) {
	return tabla_primer_nivel + (entrada_tabla - 1) * memoria_config->entradas_por_tabla;
}

uint32_t get_marco_de_pagina(uint32_t tabla_segundo_nivel, uint32_t entrada_tabla) {
	uint32_t indice = tabla_segundo_nivel + (entrada_tabla - 1);
	t_pagina_segundo_nivel *pagina = (t_pagina_segundo_nivel *)list_get(tablas_de_paginacion, indice);
	// TODO: validar pagina en memoria con bit presencia en 1 si esta.

	return pagina->marco;
}
