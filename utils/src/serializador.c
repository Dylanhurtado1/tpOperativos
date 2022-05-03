#include "serializador.h"

void print_instrucciones(t_list *instrucciones, t_log *logger);
void print_pcb(t_pcb *pcb, t_log *logger);

t_paquete *serializar_instrucciones(t_list *instrucciones, t_protocolo protocolo) {
	t_paquete *paquete = crear_paquete(protocolo, buffer_vacio());

	for(int i = 0; i < list_size(instrucciones); i++) {
		t_instruccion *instruccion = (t_instruccion *)list_get(instrucciones, i);
		agregar_a_paquete(paquete, &(instruccion->identificador), sizeof(t_identificador));
		agregar_a_paquete(paquete, &(instruccion->primer_operando), sizeof(uint32_t));
		agregar_a_paquete(paquete, &(instruccion->segundo_operando), sizeof(uint32_t));
	}

	return paquete;
}

t_list *deserializar_instrucciones(t_list *datos, t_log *logger) {
	t_list *lista_instrucciones = list_create();

	for(int i = 0; i < list_size(datos) - 1; i += 3) {
		t_instruccion *instruccion = malloc(sizeof(t_instruccion));
		instruccion->identificador = *(t_identificador *)list_get(datos, i);
		instruccion->primer_operando = *(uint32_t *)list_get(datos, i + 1);
		instruccion->segundo_operando = *(uint32_t *)list_get(datos, i + 2);
		list_add(lista_instrucciones, instruccion);
	}
	print_instrucciones(lista_instrucciones, logger);

	return lista_instrucciones;
}

void print_instrucciones(t_list *instrucciones, t_log *logger) {
	log_info(logger,"Cantidad de instrucciones recibidas: %d", list_size(instrucciones));
	for(int i = 0; i < list_size(instrucciones); i++) {
		t_instruccion *instruccion = (t_instruccion *)list_get(instrucciones, i);
		log_info(logger, "ID: %d, OP_1: %d, OP_2: %d",
				instruccion->identificador, instruccion->primer_operando, instruccion->segundo_operando);
	}
}

t_paquete *serializar_pcb(t_pcb *proceso, t_protocolo protocolo) {
	t_paquete *paquete = crear_paquete(protocolo, buffer_vacio());
	agregar_a_paquete(paquete, &(proceso->id), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(proceso->tam_proceso), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(proceso->program_counter), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(proceso->tabla_paginas), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(proceso->estimacion_rafaga), sizeof(uint32_t));

	for(int i = 0; i < list_size(proceso->instrucciones); i++) {
		t_instruccion *instruccion = (t_instruccion *)list_get(proceso->instrucciones, i);
		agregar_a_paquete(paquete, &(instruccion->identificador), sizeof(t_identificador));
		agregar_a_paquete(paquete, &(instruccion->primer_operando), sizeof(uint32_t));
		agregar_a_paquete(paquete, &(instruccion->segundo_operando), sizeof(uint32_t));
	}

	return paquete;
}

t_pcb *deserializar_pcb(t_paquete *paquete, t_log *logger) {
	t_list *datos = deserealizar_paquete(paquete);
	t_pcb *pcb = malloc(sizeof(t_pcb));

	pcb->id = *(uint32_t *)list_get(datos, 0);
	pcb->tam_proceso = *(uint32_t *)list_get(datos, 1);
	pcb->program_counter = *(uint32_t *)list_get(datos, 2);
	pcb->tabla_paginas = *(uint32_t *)list_get(datos, 3);
	pcb->estimacion_rafaga = *(uint32_t *)list_get(datos, 4);

	pcb->instrucciones = list_create();
	for(int i = 5; i < list_size(datos); i += 3) {
		t_instruccion *instruccion = malloc(sizeof(t_instruccion));
		instruccion->identificador = *(t_identificador *)list_get(datos, i);
		instruccion->primer_operando = *(uint32_t *)list_get(datos, i + 1);
		instruccion->segundo_operando = *(uint32_t *)list_get(datos, i + 2);
		list_add(pcb->instrucciones, instruccion);
	}
	print_pcb(pcb, logger);

	list_destroy_and_destroy_elements(datos, free);

	return pcb;
}

void print_pcb(t_pcb *pcb, t_log *logger) {
	log_info(logger, "PCB ID = %d", pcb->id);
	log_info(logger, "PCB Tamanio proceso = %d", pcb->tam_proceso);
	log_info(logger, "PCB Program counter = %d", pcb->program_counter);
	log_info(logger, "PCB Tabla de paginas = %d", pcb->tabla_paginas);
	log_info(logger, "PCB Estimacion rafaga = %d", pcb->estimacion_rafaga);
	log_info(logger, "PCB cantidad de instrucciones = %d", list_size(pcb->instrucciones));
}


t_traductor *deserializar_traductor(t_paquete *paquete) {
	t_list *datos = deserealizar_paquete(paquete);
	t_traductor *traductor = malloc(sizeof(t_traductor));
	traductor->cantidad_entradas_tabla = *(uint32_t *)list_get(datos, 0);
	traductor->tamanio_pagina = *(uint32_t *)list_get(datos, 1);

	list_destroy_and_destroy_elements(datos, free);

	return traductor;
}
