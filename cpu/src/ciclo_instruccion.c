#include "ciclo_instruccion.h"

uint32_t tiempo_io;


void ejecutar_ciclo_de_instruccion(t_pcb *pcb, int socket_kernel) {
	t_desalojo tipo_desalojo;
	t_instruccion *proxima_instruccion;

	do {
		proxima_instruccion = fetch(pcb);
		if(decode(proxima_instruccion)) {
			proxima_instruccion->segundo_operando = fetch_operands(pcb->tabla_paginas, proxima_instruccion->segundo_operando);
		}
		tipo_desalojo = execute(proxima_instruccion, pcb->tabla_paginas);
		if(check_interrupt()) {
			tipo_desalojo = DESALOJO_INTERRUPCION;
		}
	} while(tipo_desalojo == NO_DESALOJAR);

	desalojar_proceso(pcb, tipo_desalojo, socket_kernel);

}

t_instruccion *fetch(t_pcb *pcb) {
	uint32_t pc = pcb->program_counter;
	t_instruccion *instruccion = (t_instruccion *)list_get(pcb->instrucciones, pc);
	pcb->program_counter++;
	return instruccion;
}

bool decode(t_instruccion *proxima_instruccion) {
	return proxima_instruccion->identificador == COPY;
}

uint32_t fetch_operands(uint32_t tabla_primer_nivel, uint32_t direccion_logica) {
	// TODO: Buscar en memoria
	//leer_de_memoria(tabla_primer_nivel, direccion_logica);
	return 10;
}

t_desalojo execute(t_instruccion *instruccion, uint32_t tabla_primer_nivel) {
	switch(instruccion->identificador) {
		case NO_OP:
			log_info(cpu_logger, "Instruccion NO_OP ejecutada...");
			usleep(cpu_config->retardo_noop * 1000);
			break;
		case IO:
			log_info(cpu_logger, "Instruccion IO ejecutada...");
			tiempo_io = instruccion->primer_operando;
			return DESALOJO_IO;
		case READ:
			log_info(cpu_logger, "Instruccion READ ejecutada...");
			uint32_t valor = mmu_leer_memoria(tabla_primer_nivel, instruccion->primer_operando);
			log_info(cpu_logger, "Valor leido de memoria = %d", valor);
			break;
		case WRITE:
			log_info(cpu_logger, "Instruccion WRITE ejecutada...");
			//direccion_logica=instruccion->primer_operando;
			//exec_instruccion_WRITE(direccion_logica, valor);
			break;
		case COPY:
			log_info(cpu_logger, "Instruccion COPY ejecutada...");
			//direccion_logica = instruccion->primer_operando;
			//exec_instruccion_COPY(direccion_logica, valor);
			break;
		case EXIT:
			log_info(cpu_logger, "Instruccion EXIT ejecutada...");
			return DESALOJO_EXIT;
		default:
			break;
	}
	return NO_DESALOJAR;
}

bool check_interrupt() {
	pthread_mutex_lock(&mutex_interrupt);
	bool status = interrupcion_desalojo;
	if(interrupcion_desalojo) {
		interrupcion_desalojo = false;
	}
	pthread_mutex_unlock(&mutex_interrupt);

	return status;
}

void desalojar_proceso(t_pcb *pcb, t_desalojo tipo_desalojo, int socket_kernel) {
	t_paquete *paquete;
	switch(tipo_desalojo) {
		case DESALOJO_IO:
			paquete = serializar_pcb(pcb, BLOQUEAR_PROCESO);
			break;
		case DESALOJO_EXIT:
			paquete = serializar_pcb(pcb, FINALIZAR_PROCESO);
			break;
		case DESALOJO_INTERRUPCION:
			paquete = serializar_pcb(pcb, PROCESO_DESALOJADO);
			break;
		default:
			break;

	}
	enviar_paquete(paquete, socket_kernel);
	eliminar_paquete(paquete);
	if(tipo_desalojo == DESALOJO_IO) {
		enviar_datos(socket_kernel, &tiempo_io, sizeof(uint32_t));
	}
}




