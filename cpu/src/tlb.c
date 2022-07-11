#include "tlb.h"

static void reemplazar_entrada_tlb(t_tlb *entrada_a_agregar, char *algoritmo_reemplazo);
static void eliminar_entrada_tlb(t_tlb *entrada);
static void limpiar_entrada_swapeada(uint32_t marco);
static bool existe_marco(uint32_t marco);
static void print_tlb();

uint32_t indice_ultima_referencia = 0;


bool tlb_hit(uint32_t pagina) {//esa pagina ya esta en la tlb?

	bool existe_pagina(void *entrada) {
		return ((t_tlb *)entrada)->pagina == pagina;
	}
	return !list_is_empty(tlb) && list_any_satisfy(tlb, existe_pagina);
}

uint32_t tlb_marco_de_pagina(uint32_t pagina) {//segun el numero de pagina, busca el marco correspondiente

	bool existe_pagina(void *entrada) {//solo verifica si la pagina esta en la tlb, pagina es un numero
		return ((t_tlb *)entrada)->pagina == pagina;
	}
	t_tlb *entrada = (t_tlb *)list_find(tlb, existe_pagina);//comprueba si en la lista tlb, esta la pagina
	entrada->ultima_referencia = indice_ultima_referencia;//inicia en 0
	indice_ultima_referencia++;//y se incrementa
	print_tlb();

	return entrada->marco;//devuelve el marco de esa pagina
}

void tlb_agregar_entrada(uint32_t pagina, uint32_t marco) {//con la pagina y marco se pone en la tlb
	t_tlb *entrada = malloc(sizeof(t_tlb));
	entrada->pagina = pagina;
	entrada->marco = marco;
	entrada->ultima_referencia = indice_ultima_referencia;
	indice_ultima_referencia++;//esto preguntarle a cesar

	if(existe_marco(marco)) {//solo chequea si ese marco ya esta en la tlb
		limpiar_entrada_swapeada(marco);//se elimina esa entrada
	}

	if(list_size(tlb) < cpu_config->entradas_tlb) {//el tamaño de la lista tlb < entradas del config
		list_add(tlb, entrada);//se agrega a la tlb esa nueva entrada, osea esa nueva tlb -> entrada == tlb
	} else {//si es mayor, no se puede agregar esa entrada, asi que hay que reemplazar una entrada de la tlb
		reemplazar_entrada_tlb(entrada, cpu_config->reemplazo_tlb);//segun lru o fifo, agrego la nueva entrada
	}
	print_tlb();//imprimo el estado de la tlb
}

void tlb_limpiar_cache() {
	list_clean_and_destroy_elements(tlb, (void *)eliminar_entrada_tlb);
	indice_ultima_referencia = 0;
}


static void reemplazar_entrada_tlb(t_tlb *entrada_a_agregar, char *algoritmo_reemplazo) {//segun fifo o lru, se agrega una nueva entrada a la lista tlb
	if(string_equals_ignore_case(algoritmo_reemplazo, "FIFO")) {//si es fifo
		list_remove_and_destroy_element(tlb, 0, (void *)eliminar_entrada_tlb);//elimina la entrada 0
		list_add(tlb, entrada_a_agregar);//y se agrega a la tlb la nueva entrada
	} else if(string_equals_ignore_case(algoritmo_reemplazo, "LRU")) {//si es lru
		void *menos_usado(void *e1, void *e2) {//el menos usado se fija las referencias, es un contador para cada entrada
			return ((t_tlb *)e1)->ultima_referencia < ((t_tlb *)e2)->ultima_referencia ? e1 : e2;
		}
		t_tlb *aux = list_get_minimum(tlb, menos_usado);//de toda la lista de tlb, devuelve la menos usada
		aux->pagina = entrada_a_agregar->pagina;//igualo al aux la entrada a agregar
		aux->marco = entrada_a_agregar->marco;
		aux->ultima_referencia = entrada_a_agregar->ultima_referencia;
		eliminar_entrada_tlb(entrada_a_agregar);//se agrega aux a la tlb,y la entrada se elimina,
		//porque no hay list_add(tlb,aux)??
	}
}

static void eliminar_entrada_tlb(t_tlb *entrada) {//se eliminar esa entrada,es un free nomas
	free(entrada);
}

static bool existe_marco(uint32_t marco) {//solo pregunta si ese marco ya esta en la tlb
	bool numero_marco(t_tlb *entrada) {
		return entrada->marco == marco;
	}
	return list_any_satisfy(tlb, (void *)numero_marco);
}

static void limpiar_entrada_swapeada(uint32_t marco) {//si ya esta ese marco en la tlb, se elimina esa entrada
	bool numero_marco(t_tlb *entrada) {
		return entrada->marco == marco;
	}
	list_remove_and_destroy_by_condition(tlb, (void *)numero_marco, (void *)eliminar_entrada_tlb);//elimina esa entrada solo si ya esta ese marco
}

static void print_tlb() {//se imprime en el terminal, q es ultima referencia?
	void print(t_tlb *entrada) {
		log_info(cpu_logger, "TLB = [Page %d | Frame %d | t_uso %d]", entrada->pagina, entrada->marco, entrada->ultima_referencia);
	}
	list_iterate(tlb, (void *)print);//hace el print en toda la tlb
}

