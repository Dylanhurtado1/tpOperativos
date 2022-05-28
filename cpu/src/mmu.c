#include "mmu.h"


uint32_t leer_de_memoria(uint32_t tabla_primer_nivel, uint32_t direccion_logica) {
	// TODO: pedir tabla segundo nivel tabla_primer_nivel
	// TODO: pedir marco de memoria con tabla_segundo_nivel
}



uint32_t traducir_direccion(uint32_t direcion_logica) {
	// ACA SE IMPLEMENTARIA EL PROCESO DE TRADUCCION DE DIR LOGICA A FISICA
	/*
	int numero_pagina = dir_logica/config_memoria->tamanio_pagina;
	int entrada_tabla_1er_nivel = numero_pagina/config_cpu->cantidad_entradas_por_tabla;
	int entrada_tabla_2do_nivel = numero_pagina mod (config_cpu->cantidad_entradas_por_tabla);
	int desplazamiento = dir_logica-numero_pagina*tamanio_pagina;

	// CREAR ESTRUCTURA TLB en otra funcion posiblemente
*/
	return 0;
}



void exec_instruccion_READ (int direccion_logica){
	//int direccion_fisica = traducir_direccion(direccion_logica);
	// traduccion y acceso a memoria para LEER
}

void exec_instruccion_WRITE (int dir_logica, int valor){
	// traduccion y acceso a memoria para ESCRIBIR
}

void exec_instruccion_COPY (int dir_logica_destino, int dir_logica_origen){

}
