#include "paginacion.h"

/*typedef struct {
	uint32_t pid;
	t_list *tabla_primer_nivel;
} t_tabla_paginacion;
*/
/*typedef struct {
	uint32_t pid;
	uint32_t indice;
	t_list *tabla_segundo_nivel;
} t_entrada_primer_nivel;
*/
typedef struct {
	uint32_t pid;
	//uint32_t indice;
	uint32_t marco;
	uint8_t presencia;
	uint8_t usado;
	uint8_t modificado;
} t_pagina_segundo_nivel;

t_list *tablas_de_paginacion; // TODO: inicializar
uint32_t indice_tabla_primer_nivel; // TODO: inicializar

uint32_t crear_tablas_de_paginacion(t_pcb *pcb) {

	/*for(int i = 0; i < memoria_config->entradas_por_tabla; i++) {
		t_entrada_primer_nivel *entrada_primer_nivel = malloc(sizeof(t_entrada_primer_nivel));
		entrada_primer_nivel->pid = pcb->id;
		entrada_primer_nivel->indice = i;
		entrada_primer_nivel->tabla_segundo_nivel = list_create();

		for(int j = 0; j < memoria_config->entradas_por_tabla; j++) {
			t_entrada_segundo_nivel *entrada_segundo_nivel = malloc(sizeof(t_entrada_segundo_nivel));
			entrada_segundo_nivel->indice = j;
			entrada_segundo_nivel->marco = 0;
			entrada_segundo_nivel->presencia = 0;
			entrada_segundo_nivel->usado = 0;
			entrada_segundo_nivel->modificado = 0;
			list_add(entrada_primer_nivel->tabla_segundo_nivel, entrada_segundo_nivel);
		}
		list_add(tablas_de_paginacion, entrada_primer_nivel);
	}*/

	uint32_t cantidad_entradas = memoria_config->entradas_por_tabla * memoria_config->entradas_por_tabla;

	for(int i = 0; i < cantidad_entradas; i++) {
		t_pagina_segundo_nivel *entrada_segundo_nivel = malloc(sizeof(t_pagina_segundo_nivel));
		entrada_segundo_nivel->pid = pcb->id;
		entrada_segundo_nivel->marco = 23;
		entrada_segundo_nivel->presencia = 0;
		entrada_segundo_nivel->usado = 0;
		entrada_segundo_nivel->modificado = 0;
		list_add(tablas_de_paginacion, entrada_segundo_nivel);

	}
	indice_tabla_primer_nivel += cantidad_entradas;
	return indice_tabla_primer_nivel % cantidad_entradas;
}

uint32_t get_tabla_segundo_nivel(uint32_t tabla_primer_nivel, uint32_t entrada_tabla) {
	return tabla_primer_nivel + entrada_tabla * memoria_config->entradas_por_tabla;
}

uint32_t get_marco_de_pagina(uint32_t tabla_segundo_nivel, uint32_t entrada_tabla) {
	uint32_t indice = tabla_segundo_nivel + entrada_tabla;
	t_pagina_segundo_nivel *pagina = (t_pagina_segundo_nivel *)list_get(tablas_de_paginacion, indice);
	return pagina->marco;
}
