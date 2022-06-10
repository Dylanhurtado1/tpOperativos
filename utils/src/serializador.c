#include "serializador.h"

t_paquete *serializar_instrucciones(t_list *instrucciones, t_protocolo protocolo);
t_list *deserializar_instrucciones(t_list *datos, uint32_t longitud_datos);


t_paquete *serializar_consola(t_list *instrucciones, uint32_t tamanio_consola, t_protocolo protocolo) {
	t_paquete *paquete = serializar_instrucciones(instrucciones, protocolo);
	agregar_a_paquete(paquete, &tamanio_consola, sizeof(uint32_t));

	return paquete;
}

t_consola *deserializar_consola(t_paquete *paquete) {
	t_list *datos = deserealizar_paquete(paquete);
	t_consola *consola = malloc(sizeof(t_consola));

	consola->instrucciones = deserializar_instrucciones(datos, list_size(datos) - 1);
	consola->tamanio = *(uint32_t *)list_get(datos, list_size(datos) - 1);

	list_destroy_and_destroy_elements(datos, free);
	return consola;
}

t_paquete *serializar_pcb(t_pcb *pcb, t_protocolo protocolo) {
	t_paquete *paquete = serializar_instrucciones(pcb->instrucciones, protocolo);
	agregar_a_paquete(paquete, &(pcb->id), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(pcb->tamanio_proceso), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(pcb->program_counter), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(pcb->tabla_paginas), sizeof(uint32_t));
	agregar_a_paquete(paquete, &(pcb->estimacion_rafaga), sizeof(uint32_t));

	return paquete;
}

t_pcb *deserializar_pcb(t_paquete *paquete) {
	t_list *datos = deserealizar_paquete(paquete);
	t_pcb *pcb = malloc(sizeof(t_pcb));

	pcb->instrucciones = deserializar_instrucciones(datos, list_size(datos) - 5);

	uint32_t offset = list_size(pcb->instrucciones) * 3;
	pcb->id = *(uint32_t *)list_get(datos, offset + 0);
	pcb->tamanio_proceso = *(uint32_t *)list_get(datos, offset + 1);
	pcb->program_counter = *(uint32_t *)list_get(datos, offset + 2);
	pcb->tabla_paginas = *(uint32_t *)list_get(datos, offset + 3);
	pcb->estimacion_rafaga = *(uint32_t *)list_get(datos, offset + 4);

	list_destroy_and_destroy_elements(datos, free);
	return pcb;
}

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

t_list *deserializar_instrucciones(t_list *datos, uint32_t longitud_datos) {
	t_list *instrucciones = list_create();

	for(int i = 0; i < longitud_datos; i += 3) {
		t_instruccion *instruccion = malloc(sizeof(t_instruccion));
		instruccion->identificador = *(t_identificador *)list_get(datos, i);
		instruccion->primer_operando = *(uint32_t *)list_get(datos, i + 1);
		instruccion->segundo_operando = *(uint32_t *)list_get(datos, i + 2);
		list_add(instrucciones, instruccion);
	}

	return instrucciones;
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

t_paquete *serializar_tabla_de_acceso(uint32_t direccion, uint32_t entrada, t_protocolo protocolo) {
	t_paquete *paquete = crear_paquete(protocolo, buffer_vacio());
	agregar_a_paquete(paquete, &direccion, sizeof(uint32_t));
	agregar_a_paquete(paquete, &entrada, sizeof(uint32_t));
	return paquete;
}

t_tabla_acceso *deserializar_tabla_de_acceso(t_paquete *paquete) {
	t_list *datos = deserealizar_paquete(paquete);
	t_tabla_acceso *tabla_paginas = malloc(sizeof(t_tabla_acceso));

	tabla_paginas->direccion = *(uint32_t *)list_get(datos, 0);
	tabla_paginas->entrada = *(uint32_t *)list_get(datos, 1);

	list_destroy_and_destroy_elements(datos, free);
	return tabla_paginas;
}



