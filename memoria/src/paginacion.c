#include "paginacion.h"

static void asignar_marco_libre(t_pagina_segundo_nivel *pagina);
static void reemplazar_pagina(t_pagina_segundo_nivel *pagina_a_agregar, t_puntero_clock *puntero, char *algoritmo_reemplazo);
static t_list *paginas_cargadas_en_memoria(uint32_t pid);
static t_pagina_segundo_nivel *buscar_pagina_victima(t_list *paginas_cargadas, t_puntero_clock *puntero, char *algoritmo_reemplazo);
static void crear_puntero_proceso(uint32_t pid);
static t_puntero_clock *buscar_puntero(uint32_t pid);

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
		entrada_segundo_nivel->numero_pagina = i;
		list_add(tablas_de_paginacion, entrada_segundo_nivel);
	}

	indice_tabla_primer_nivel += cantidad_entradas_maximas;

	crear_puntero_proceso(pid);

	return tabla_primer_nivel;
}

uint32_t get_tabla_segundo_nivel(uint32_t tabla_primer_nivel, uint32_t entrada_tabla) {
	return tabla_primer_nivel + entrada_tabla * memoria_config->entradas_por_tabla;
}

uint32_t get_marco_de_pagina(uint32_t tabla_segundo_nivel, uint32_t entrada_tabla) {
	uint32_t indice_pagina = tabla_segundo_nivel + entrada_tabla;
	t_pagina_segundo_nivel *pagina = (t_pagina_segundo_nivel *)list_get(tablas_de_paginacion, indice_pagina);

	if(page_fault(pagina)) {
		t_puntero_clock *puntero = buscar_puntero(pagina->pid);
		if(cantidad_marcos_asignados(pagina->pid) < memoria_config->marcos_por_proceso) {
			asignar_marco_libre(pagina);

			puntero->indice_marco = 0;
		} else {
			// TODO: evaluar que datos se necesitan
			reemplazar_pagina(pagina, puntero, memoria_config->algoritmo_reemplazo);
		}
		swap_out(pagina->pid, pagina->numero_pagina, pagina->marco);

		pagina->presencia = 1;
	}

	pagina->uso = 1;

	log_info(memoria_logger, "Page pedido %d = [Frame %d | P %d | U %d | M %d]",
			pagina->numero_pagina, pagina->marco, pagina->presencia, pagina->uso, pagina->modificado);

	return pagina->marco;
}

void actualizar_pagina_modificada(t_marco *marco_modificado) {
	bool marco_asignado(t_pagina_segundo_nivel *pagina) {
		return pagina->marco == marco_modificado->numero && pagina->pid == marco_modificado->pid;
	}
	t_pagina_segundo_nivel *pagina_modificada = list_find(tablas_de_paginacion, (void *)marco_asignado);

	pagina_modificada->modificado = 1;
}

void liberar_paginas_cargadas(uint32_t pid) {
	t_list *paginas_en_memoria = paginas_cargadas_en_memoria(pid);

	void liberar_pagina(t_pagina_segundo_nivel *pagina) {
		if(pagina_modificada(pagina)) {
			swap_in(pagina->pid, pagina->numero_pagina, pagina->marco);
			pagina->modificado = 0;
		}
		pagina->presencia = 0;
	}
	list_iterate(paginas_en_memoria, (void *)liberar_pagina);
	list_destroy(paginas_en_memoria);

	t_puntero_clock *puntero = buscar_puntero(pid);
	puntero->indice_marco = 0;
}

bool page_fault(t_pagina_segundo_nivel *pagina) {
	return pagina->presencia == 0;
}

bool pagina_modificada(t_pagina_segundo_nivel *pagina) {
	return pagina->modificado == 1;
}

bool pagina_presente(t_pagina_segundo_nivel *pagina) {
	return pagina->presencia == 1;
}

bool pagina_en_uso(t_pagina_segundo_nivel *pagina) {
	return pagina->uso == 1;
}


static void reemplazar_pagina(t_pagina_segundo_nivel *pagina_a_agregar, t_puntero_clock *puntero, char *algoritmo_reemplazo) {
	t_list *paginas_en_memoria = paginas_cargadas_en_memoria(pagina_a_agregar->pid);

	t_pagina_segundo_nivel *pagina_victima = buscar_pagina_victima(paginas_en_memoria, puntero, algoritmo_reemplazo);

	if(pagina_modificada(pagina_victima)) {
		swap_in(pagina_victima->pid, pagina_victima->numero_pagina, pagina_victima->marco);
		pagina_victima->modificado = 0;
	}
	pagina_a_agregar->marco = pagina_victima->marco;
	pagina_victima->presencia = 0;

	list_destroy(paginas_en_memoria);
}

void print_paginas_memoria(t_list *paginas_en_memoria) { // TODO: solo para debug
	void print(t_pagina_segundo_nivel *pagina) {
		log_info(memoria_logger, "Page %d = [Frame %d | P %d | U %d | M %d]",
				pagina->numero_pagina, pagina->marco, pagina->presencia, pagina->uso, pagina->modificado);
	}
	list_iterate(paginas_en_memoria, (void *)print);
}

static t_list *paginas_cargadas_en_memoria(uint32_t pid) {
	bool esta_cargada(t_pagina_segundo_nivel *pagina) {
		return pagina_presente(pagina) && pagina->pid == pid;
	}
	t_list *paginas = list_filter(tablas_de_paginacion, (void *)esta_cargada);

	print_paginas_memoria(paginas);

	return paginas;
}

static t_pagina_segundo_nivel *buscar_pagina_victima(t_list *paginas_cargadas, t_puntero_clock *puntero, char *algoritmo_reemplazo) {
	t_pagina_segundo_nivel *victima = NULL;

	bool por_numero_marco(void *p1, void *p2) {
		return ((t_pagina_segundo_nivel *)p1)->marco < ((t_pagina_segundo_nivel *)p2)->marco;
	}
	list_sort(paginas_cargadas, (void *)por_numero_marco);

	if(string_equals_ignore_case(algoritmo_reemplazo, "CLOCK")) {
		do {
			for(int i = puntero->indice_marco; i < list_size(paginas_cargadas); i++) {
				t_pagina_segundo_nivel *pagina = list_get(paginas_cargadas, i);
				if(pagina_en_uso(pagina)) {
					pagina->uso = 0;
				} else {
					puntero->indice_marco = (i + 1) % list_size(paginas_cargadas);
					victima = pagina;
					break;
				}
			}
			if(victima == NULL) {
				puntero->indice_marco = 0;
			}
		} while(victima == NULL);
	} else if(string_equals_ignore_case(algoritmo_reemplazo, "CLOCK-M")) {
		int paso = 1;
		int paginas_recorridas = 0;
		uint32_t aux_indice = puntero->indice_marco;
		do {
			for(int i = aux_indice; i < list_size(paginas_cargadas) && paginas_recorridas < list_size(paginas_cargadas); i++) {
				t_pagina_segundo_nivel *pagina = list_get(paginas_cargadas, i);
				switch(paso) {
					case 1:
						victima = !pagina_en_uso(pagina) && !pagina_modificada(pagina) ? pagina : NULL;
						break;
					case 2:
						victima = !pagina_en_uso(pagina) && pagina_modificada(pagina) ? pagina : NULL;
						pagina->uso = 0;
						break;
				}
				if(victima != NULL) {
					puntero->indice_marco = (i + 1) % list_size(paginas_cargadas);
					break;
				}
				paginas_recorridas++;
			}
			aux_indice = 0;

			if(victima == NULL && paginas_recorridas >= list_size(paginas_cargadas)) {
				paso = paso == 1 ? 2 : 1;
				paginas_recorridas = 0;
				aux_indice = puntero->indice_marco;
			}
		} while(victima == NULL);
	}

	return victima;
}

static void asignar_marco_libre(t_pagina_segundo_nivel *pagina) {
	t_marco *marco = buscar_marco_libre();
	pagina->marco = marco->numero;
	marco->pid = pagina->pid;
	marco->libre = 0;
}

static void crear_puntero_proceso(uint32_t pid) {
	t_puntero_clock *puntero = malloc(sizeof(t_puntero_clock));
	puntero->pid = pid;
	puntero->indice_marco = 0;

	list_add(punteros_clock, puntero);
}

static t_puntero_clock *buscar_puntero(uint32_t pid) {
	bool mismo_pid(t_puntero_clock *clock) {
		return clock->pid == pid;
	}
	return list_find(punteros_clock, (void *)mismo_pid);
}

