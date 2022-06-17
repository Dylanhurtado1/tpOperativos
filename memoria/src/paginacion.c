#include "paginacion.h"

static uint32_t calcular_offset_pagina(uint32_t indice_pagina);
static void cargar_espacio_en_memoria(void *buffer, uint32_t offset, uint32_t size);


uint32_t indice_tabla_primer_nivel = 0;


uint32_t crear_tablas_de_paginacion(uint32_t pid) {
	uint32_t tabla_primer_nivel = indice_tabla_primer_nivel;
	uint32_t cantidad_entradas_maximas = memoria_config->entradas_por_tabla * memoria_config->entradas_por_tabla;

	for(int i = 0; i < cantidad_entradas_maximas; i++) {
		t_pagina_segundo_nivel *entrada_segundo_nivel = malloc(sizeof(t_pagina_segundo_nivel));
		entrada_segundo_nivel->pid = pid;
		entrada_segundo_nivel->marco = 0xFF;
		entrada_segundo_nivel->presencia = 0;
		entrada_segundo_nivel->uso = 0;
		entrada_segundo_nivel->modificado = 0;
		list_add(tablas_de_paginacion, entrada_segundo_nivel);
	}

	indice_tabla_primer_nivel += cantidad_entradas_maximas;

	return tabla_primer_nivel;
}

uint32_t get_tabla_segundo_nivel(uint32_t tabla_primer_nivel, uint32_t entrada_tabla) {
	return tabla_primer_nivel + entrada_tabla * memoria_config->entradas_por_tabla;
}

uint32_t get_marco_de_pagina(uint32_t tabla_segundo_nivel, uint32_t entrada_tabla) {
	uint32_t indice_pagina = tabla_segundo_nivel + entrada_tabla;
	t_pagina_segundo_nivel *pagina = (t_pagina_segundo_nivel *)list_get(tablas_de_paginacion, indice_pagina);

	if(page_fault(pagina)) {
		void *buffer = malloc(memoria_config->tamanio_pagina);
		swap_leer_pagina(pagina->pid, buffer, calcular_offset_pagina(indice_pagina), memoria_config->tamanio_pagina);
		if(cantidad_marcos_asignados(pagina->pid) < memoria_config->marcos_por_proceso) {
			asignar_marco_libre(pagina);
		} else {
			// TODO: evaluar que datos se necesitan
			reemplazar_marco(indice_pagina, pagina, memoria_config->algoritmo_reemplazo);

			//asignar_marco_libre(pagina); // <-- solo para debug, elimnar al implementar los algoritmos
		}
		cargar_espacio_en_memoria(buffer, pagina->marco * memoria_config->tamanio_pagina, memoria_config->tamanio_pagina);
		free(buffer);
		pagina->presencia = 1;
		pagina->uso = 1;
	}
	log_info(memoria_logger, "Page %d = [Frame %d | P %d | U %d | M %d]",
			indice_pagina, pagina->marco, pagina->presencia, pagina->uso, pagina->modificado);

	return pagina->marco;
}

void iniciar_marcos_memoria() {
	marcos_memoria = list_create();
	uint32_t marcos_totales = memoria_config->tamanio_memoria / memoria_config->tamanio_pagina;
	for(int i = 0; i < marcos_totales; i++) {
		t_marco *marco = malloc(sizeof(t_marco));
		marco->pid = 0xFF;
		marco->libre = 1;
		marco->numero = i;
		list_add(marcos_memoria, marco);
	}
}

void liberar_espacio_en_memoria(uint32_t pid) {
	bool esta_asignado(t_marco *marco) {
		return marco->pid == pid;
	}

	void liberar_marco(t_marco *marco) {
		marco->pid = 0xFF;
		marco->libre = 1;
	}

	t_list *marcos_asignados = list_filter(marcos_memoria, (void *)esta_asignado);
	list_iterate(marcos_memoria, (void *)liberar_marco);

	list_destroy(marcos_asignados);
}

void cargar_espacio_en_memoria(void *buffer, uint32_t offset, uint32_t size) {
	memcpy(memoria_principal + offset, buffer, size);
}

bool page_fault(t_pagina_segundo_nivel *pagina) {
	return pagina->presencia == 0;
}

uint32_t cantidad_marcos_asignados(uint32_t pid) {
	bool esta_asignado(t_marco *marco) {
		return marco->pid == pid;
	}
	t_list *marcos_asignados = list_filter(marcos_memoria, (void *)esta_asignado);
	uint32_t cantidad_marcos = list_size(marcos_asignados);
	list_destroy(marcos_asignados);

	return cantidad_marcos;
}

void asignar_marco_libre(t_pagina_segundo_nivel *pagina) {
	bool esta_libre(t_marco *marco) {
		return marco->libre == 1;
	}
	t_marco *marco_libre = list_find(marcos_memoria, (void *)esta_libre);
	pagina->marco = marco_libre->numero;
	marco_libre->pid = pagina->pid;
	marco_libre->libre = 0;
}

void reemplazar_marco(uint32_t numero_pagina, t_pagina_segundo_nivel *pagina_a_agregar, char *algoritmo_reemplazo) {
	if(string_equals_ignore_case(algoritmo_reemplazo, "CLOCK")) {
		// TODO: implementar algoritmo
	} else if(string_equals_ignore_case(algoritmo_reemplazo, "CLOCK-M")) {
		// TODO: implementar algoritmo
	}
}

uint32_t calcular_offset_pagina(uint32_t indice_pagina) {
	uint32_t numero_pagina = indice_pagina % (memoria_config->entradas_por_tabla * memoria_config->entradas_por_tabla);
	return numero_pagina * memoria_config->tamanio_pagina;
}

