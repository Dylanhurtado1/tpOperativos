#include "estructuras.h"


t_instruccion *crear_instruccion(t_identificador identificador, uint32_t primer_operando, uint32_t segundo_operando) {
	t_instruccion *instruccion = malloc(sizeof(t_instruccion));
	instruccion->identificador = identificador;
	instruccion->primer_operando = primer_operando;
	instruccion->segundo_operando = segundo_operando;

	return instruccion;
}

void eliminar_instrucciones(t_list *instrucciones) {
	list_destroy_and_destroy_elements(instrucciones, free);
}

t_consola *crear_consola(t_list *instrucciones, uint32_t tamanio) {
	t_consola *consola = malloc(sizeof(t_consola));

	consola->instrucciones = instrucciones;
	consola->tamanio = tamanio;

	return consola;
}

void eliminar_consola(t_consola *consola) {
	eliminar_instrucciones(consola->instrucciones);
	free(consola);
}

t_pcb *crear_pcb(uint32_t id, uint32_t tamanio_proceso, t_list *instrucciones, uint32_t program_counter, uint32_t estimacion_rafaga) {
	t_pcb *pcb = malloc(sizeof(t_pcb));

	pcb->id = id;
	pcb->tamanio_proceso = tamanio_proceso;
	pcb->instrucciones = list_duplicate(instrucciones);
	pcb->program_counter = program_counter;
	pcb->estimacion_rafaga = estimacion_rafaga;

	return pcb;
}

void eliminar_pcb(t_pcb *pcb) {
	eliminar_instrucciones(pcb->instrucciones);
	free(pcb);
}

t_traductor *crear_traductor_direcciones(int entradas_tabla, int tamanio_pagina) {
	t_traductor *traductor = malloc(sizeof(t_traductor));
	traductor->cantidad_entradas_tabla = entradas_tabla;
	traductor->tamanio_pagina = tamanio_pagina;

	return traductor;
}

void eliminar_traductor_direcciones(t_traductor *traductor) {
	free(traductor);
}

t_tabla_acceso *crear_tabla_de_acceso(uint32_t direccion, uint32_t entrada) {
	t_tabla_acceso *tabla_acceso = malloc(sizeof(t_tabla_acceso));

	tabla_acceso->direccion = direccion;
	tabla_acceso->entrada = entrada;

	return tabla_acceso;
}

void eliminar_tabla_de_acceso(t_tabla_acceso *tabla_acceso) {
	free(tabla_acceso);
}


