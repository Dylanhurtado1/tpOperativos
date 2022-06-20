#include "paginacion.h"

static bool page_fault(t_pagina_segundo_nivel *pagina);
static bool pagina_modificada(t_pagina_segundo_nivel *pagina);
static bool pagina_presente(t_pagina_segundo_nivel *pagina);
static uint32_t calcular_offset_pagina(uint32_t indice_pagina);
static uint32_t calcular_offset_marco(uint32_t numero_marco);
static void asignar_marco_libre(t_pagina_segundo_nivel *pagina);
static void reemplazar_pagina(t_pagina_segundo_nivel *pagina_a_agregar, uint32_t incide_pagina, char *algoritmo_reemplazo);

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
			//reemplazar_pagina(pagina, indice_pagina, memoria_config->algoritmo_reemplazo); TODO: habilitar al implementar los algoritmos
		}
		cargar_marco_en_memoria(buffer, calcular_offset_marco(pagina->marco), memoria_config->tamanio_pagina);
		free(buffer);
		pagina->presencia = 1;
		pagina->uso = 1; // TODO: este flag debe ser cargado/limpiado en el algoritmo?
	}
	log_info(memoria_logger, "Page %d = [Frame %d | P %d | U %d | M %d]",
			indice_pagina, pagina->marco, pagina->presencia, pagina->uso, pagina->modificado);

	return pagina->marco;
}

void actualizar_pagina_modificada(uint32_t direccion_fisica) {
	uint32_t numero_marco = (uint32_t)(direccion_fisica / memoria_config->tamanio_pagina);

	bool igual_numero_marco(t_marco *marco) {
		return marco->numero == numero_marco;
	}
	t_marco *marco_modificado = list_find(marcos_memoria, (void *)igual_numero_marco);

	bool marco_asignado(t_pagina_segundo_nivel *pagina) {
		return pagina->marco == marco_modificado->numero && pagina->pid == marco_modificado->pid;
	}
	t_pagina_segundo_nivel *pagina_modificada = list_find(tablas_de_paginacion, (void *)marco_asignado);

	pagina_modificada->modificado = 1;
}


static bool page_fault(t_pagina_segundo_nivel *pagina) {
	return pagina->presencia == 0;
}

static bool pagina_modificada(t_pagina_segundo_nivel *pagina) {
	return pagina->modificado == 1;
}

static bool pagina_presente(t_pagina_segundo_nivel *pagina) {
	return pagina->presencia == 1;
}

static void reemplazar_pagina(t_pagina_segundo_nivel *pagina_a_agregar, uint32_t incide_pagina, char *algoritmo_reemplazo) {
	t_pagina_segundo_nivel *pagina_victima;

	bool pagina_cargada(t_pagina_segundo_nivel *pagina) {
		return pagina_presente(pagina) && pagina->pid == pagina_a_agregar->pid;
	}
	t_list *paginas_en_memoria = list_filter(tablas_de_paginacion, (void *)pagina_cargada);

	if(string_equals_ignore_case(algoritmo_reemplazo, "CLOCK")) {
		// TODO: implementar algoritmo
	} else if(string_equals_ignore_case(algoritmo_reemplazo, "CLOCK-M")) {
		// TODO: implementar algoritmo
	}

	list_destroy(paginas_en_memoria);

	if(pagina_modificada(pagina_victima)) {
		void *buffer = malloc(memoria_config->tamanio_pagina);
		leer_marco_de_memoria(buffer, calcular_offset_marco(pagina_victima->marco), memoria_config->tamanio_pagina);
		swap_escribir_pagina(pagina_victima->pid, buffer, calcular_offset_pagina(incide_pagina), memoria_config->tamanio_pagina);
		free(buffer);
	}
	pagina_a_agregar->marco = pagina_victima->marco;
	//pagina_victima->marco = 0xFF; // TODO: es necesario escribir un valor invalido?
	pagina_victima->presencia = 0;
	//pagina_victima->uso = 0; // TODO: bit que se limpia en el algoritmo?
	//pagina_victima->modificado = 0; // TODO: bit que se limpia en el algoritmo?
}

static void asignar_marco_libre(t_pagina_segundo_nivel *pagina) {
	t_marco *marco = buscar_marco_libre();
	pagina->marco = marco->numero;
	marco->pid = pagina->pid;
	marco->libre = 0;
}

static uint32_t calcular_offset_pagina(uint32_t indice_pagina) {
	uint32_t numero_pagina = indice_pagina % (memoria_config->entradas_por_tabla * memoria_config->entradas_por_tabla);
	return numero_pagina * memoria_config->tamanio_pagina;
}

static uint32_t calcular_offset_marco(uint32_t numero_marco) {
	return numero_marco * memoria_config->tamanio_pagina;
}


