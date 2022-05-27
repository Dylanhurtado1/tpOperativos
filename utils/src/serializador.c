#include "serializador.h"


t_paquete *serializar_consola(t_list *instrucciones, uint32_t tamanio_consola, t_protocolo protocolo) {
	t_paquete *paquete = crear_paquete(protocolo, buffer_vacio());
	for(int i = 0; i < list_size(instrucciones); i++) {
		t_instruccion *instruccion = (t_instruccion *)list_get(instrucciones, i);
		agregar_a_paquete(paquete, &(instruccion->identificador), sizeof(t_identificador));
		agregar_a_paquete(paquete, &(instruccion->primer_operando), sizeof(uint32_t));
		agregar_a_paquete(paquete, &(instruccion->segundo_operando), sizeof(uint32_t));
	}
	agregar_a_paquete(paquete, &tamanio_consola, sizeof(uint32_t));

	return paquete;
}

t_consola *deserializar_consola(t_paquete *paquete) {
	t_list *datos = deserealizar_paquete(paquete);
	t_consola *consola = malloc(sizeof(t_consola));
	consola->instrucciones = list_create();

	for(int i = 0; i < list_size(datos) - 1; i += 3) {
		t_instruccion *instruccion = malloc(sizeof(t_instruccion));
		instruccion->identificador = *(t_identificador *)list_get(datos, i);
		instruccion->primer_operando = *(uint32_t *)list_get(datos, i + 1);
		instruccion->segundo_operando = *(uint32_t *)list_get(datos, i + 2);
		list_add(consola->instrucciones, instruccion);
	}

	consola->tamanio = *(uint32_t *)list_get(datos, list_size(datos) - 1);
	list_destroy_and_destroy_elements(datos, free);
	return consola;
}


/*t_paquete *serializar_instrucciones(t_list *instrucciones, t_protocolo protocolo) {
	t_paquete *paquete = crear_paquete(protocolo, buffer_vacio());

	for(int i = 0; i < list_size(instrucciones); i++) {
		t_instruccion *instruccion = (t_instruccion *)list_get(instrucciones, i);
		agregar_a_paquete(paquete, &(instruccion->identificador), sizeof(t_identificador));
		agregar_a_paquete(paquete, &(instruccion->primer_operando), sizeof(uint32_t));
		agregar_a_paquete(paquete, &(instruccion->segundo_operando), sizeof(uint32_t));
	}

	return paquete;
}

t_list *deserializar_instrucciones(t_list *datos) {
	t_list *lista_instrucciones = list_create();

	for(int i = 0; i < list_size(datos) - 1; i += 3) {
		t_instruccion *instruccion = malloc(sizeof(t_instruccion));
		instruccion->identificador = *(t_identificador *)list_get(datos, i);
		instruccion->primer_operando = *(uint32_t *)list_get(datos, i + 1);
		instruccion->segundo_operando = *(uint32_t *)list_get(datos, i + 2);
		list_add(lista_instrucciones, instruccion);
	}

	return lista_instrucciones;
}*/

t_paquete *serializar_pcb(t_pcb *proceso, t_protocolo protocolo) {
	t_paquete *paquete = crear_paquete(protocolo, buffer_vacio());
	agregar_a_paquete(paquete, &(proceso->id), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(proceso->tamanio_proceso), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(proceso->program_counter), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(proceso->tabla_paginas), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(proceso->estimacion_rafaga), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(proceso->tiempo_io), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(proceso->tiempo_inicio_bloqueo), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(proceso->estado), sizeof(t_estado));

	for(int i = 0; i < list_size(proceso->instrucciones); i++) {
		t_instruccion *instruccion = (t_instruccion *)list_get(proceso->instrucciones, i);
		agregar_a_paquete(paquete, &(instruccion->identificador), sizeof(t_identificador));
		agregar_a_paquete(paquete, &(instruccion->primer_operando), sizeof(uint32_t));
		agregar_a_paquete(paquete, &(instruccion->segundo_operando), sizeof(uint32_t));
	}

	return paquete;
}

t_pcb *deserializar_pcb(t_list *datos) {
	t_pcb *pcb = malloc(sizeof(t_pcb));

	pcb->id = *(uint32_t *)list_get(datos, 0);
	pcb->tamanio_proceso = *(uint32_t *)list_get(datos, 1);
	pcb->program_counter = *(uint32_t *)list_get(datos, 2);
	pcb->tabla_paginas = *(uint32_t *)list_get(datos, 3);
	pcb->estimacion_rafaga = *(uint32_t *)list_get(datos, 4);
	pcb->tiempo_io = *(uint32_t *)list_get(datos, 5);
	pcb->tiempo_inicio_bloqueo = *(uint32_t *)list_get(datos, 6);
	pcb->estado = *(t_estado *)list_get(datos, 7);

	pcb->instrucciones = list_create();
	for(int i = 8; i < list_size(datos); i += 3) {
		t_instruccion *instruccion = malloc(sizeof(t_instruccion));
		instruccion->identificador = *(t_identificador *)list_get(datos, i);
		instruccion->primer_operando = *(uint32_t *)list_get(datos, i + 1);
		instruccion->segundo_operando = *(uint32_t *)list_get(datos, i + 2);
		list_add(pcb->instrucciones, instruccion);
	}

	return pcb;
}

t_paquete *serializar_traductor(t_traductor *traductor, t_protocolo protocolo) {
	t_paquete *paquete = crear_paquete(protocolo, buffer_vacio());
	agregar_a_paquete(paquete, &(traductor->cantidad_entradas_tabla), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(traductor->tamanio_pagina), sizeof(uint32_t));

	return paquete;
}

t_traductor *deserializar_traductor(t_paquete *paquete) {
	t_list *datos = deserealizar_paquete(paquete);
	t_traductor *traductor = malloc(sizeof(t_traductor));
	traductor->cantidad_entradas_tabla = *(uint32_t *)list_get(datos, 0);
	traductor->tamanio_pagina = *(uint32_t *)list_get(datos, 1);

	list_destroy_and_destroy_elements(datos, free);

	return traductor;
}



