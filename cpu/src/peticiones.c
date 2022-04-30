#include "peticiones.h"

extern t_log *cpu_logger;

void peticiones_dispatch(int *socket_dispatch) {
	log_info(cpu_logger, "CPU escuchando puerto dispatch");
	int socket_kernel = esperar_cliente(*socket_dispatch);
	while (true) {
		t_paquete *paquete = recibir_paquete(socket_kernel);
		switch (paquete->codigo_operacion) {
			case PCB:
				log_info(cpu_logger,"PCB recibida");
				deserealizar_pcb(paquete);
				break;
			default:
				log_error(cpu_logger,"Operacion desconocida.");
				break;
		}
		eliminar_paquete(paquete);
	}
}

void peticiones_interrupt(int *socket_interrupt) {
	log_info(cpu_logger, "CPU escuchando puerto interrupt");
	int socket_kernel = esperar_cliente(*socket_interrupt);
	while (true) {
		t_paquete *paquete = recibir_paquete(socket_kernel);
		switch (paquete->codigo_operacion) {
			default:
				log_error(cpu_logger,"Operacion desconocida.");
				break;
		}
		eliminar_paquete(paquete);
	}
}

t_pcb *deserealizar_pcb(t_paquete *paquete) {
	t_list *datos = deserealizar_paquete(paquete);
	log_info(cpu_logger, "Cantidad de datos = %d", list_size(datos));
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

	list_destroy_and_destroy_elements(datos, free);

	log_info(cpu_logger, "PCB ID = %d", pcb->id);
	log_info(cpu_logger, "PCB Tamanio proceso = %d", pcb->tam_proceso);
	log_info(cpu_logger, "PCB Program counter = %d", pcb->program_counter);
	log_info(cpu_logger, "PCB Tabla de paginas = %d", pcb->tabla_paginas);
	log_info(cpu_logger, "PCB Estimacion rafaga = %d", pcb->estimacion_rafaga);
	log_info(cpu_logger, "PCB cantidad de instrucciones = %d", list_size(pcb->instrucciones));

	return pcb;
}

