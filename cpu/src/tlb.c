#include "tlb.h"

static void reemplazar_entrada_tlb(t_tlb *entrada_a_agregar, char *algoritmo_reemplazo);
static void tlb_eliminar_entrada(t_tlb *entrada);
static void print_tlb();


bool tlb_existe_pagina(uint32_t pagina) {

	bool existe_pagina(void *entrada) {
		return ((t_tlb *)entrada)->pagina == pagina;
	}
	return !list_is_empty(tlb) && list_any_satisfy(tlb, existe_pagina);
}

uint32_t tlb_marco_de_pagina(uint32_t pagina) {

	bool existe_pagina(void *entrada) {
		return ((t_tlb *)entrada)->pagina == pagina;
	}
	t_tlb *entrada = (t_tlb *)list_find(tlb, existe_pagina);

	return entrada->marco;
}

void tlb_agregar_entrada(uint32_t pagina, uint32_t marco) {
	t_tlb *entrada = malloc(sizeof(t_tlb));
	entrada->pagina = pagina;
	entrada->marco = marco;

	if(list_size(tlb) < cpu_config->entradas_tlb) {
		list_add(tlb, entrada);
		print_tlb();
	} else {
		reemplazar_entrada_tlb(entrada, cpu_config->reemplazo_tlb);
		print_tlb();
	}
}

void tlb_eliminar_entradas() {
	list_clean_and_destroy_elements(tlb, free);
}


void reemplazar_entrada_tlb(t_tlb *entrada_a_agregar, char *algoritmo_reemplazo) {
	if(string_equals_ignore_case(algoritmo_reemplazo, "FIFO")) {
		list_remove_and_destroy_element(tlb, 0, (void *)tlb_eliminar_entrada);
		list_add(tlb, entrada_a_agregar);
	} else if(string_equals_ignore_case(algoritmo_reemplazo, "LRU")) {
		// TODO: implementar algoritmo
	}
}

void tlb_eliminar_entrada(t_tlb *entrada) {
	free(entrada);
}

void print_tlb() {
	void print(t_tlb *entrada) {
		log_info(cpu_logger, "Pagina = %d", entrada->pagina);
	}
	list_iterate(tlb, (void *)print);
}

