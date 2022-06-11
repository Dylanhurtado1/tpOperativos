#include "tlb.h"

static void reemplazar_entrada_tlb(t_tlb *entrada_a_agregar, char *algoritmo_reemplazo);


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
	} else {
		reemplazar_entrada_tlb(entrada, cpu_config->reemplazo_tlb);
	}
}

void tlb_eliminar_entradas() {
	list_clean_and_destroy_elements(tlb, free);
}


void reemplazar_entrada_tlb(t_tlb *entrada_a_agregar, char *algoritmo_reemplazo) {
	if(string_equals_ignore_case(algoritmo_reemplazo, "FIFO")) {
		// TODO: implementar algoritmo
	} else if(string_equals_ignore_case(algoritmo_reemplazo, "LRU")) {
		// TODO: implementar algoritmo
	}
}
