#include "ciclo_instruccion.h"


void ejecutar_ciclo_de_instruccion(t_pcb *pcb, int socket_kernel) {
	t_desalojo tipo_desalojo;
	t_instruccion *proxima_instruccion;

	do {
		proxima_instruccion = fetch(pcb);
		if(decode(proxima_instruccion)) {
			proxima_instruccion->segundo_operando = fetch_operands(pcb->tabla_paginas, proxima_instruccion->segundo_operando);
		}
		pcb->program_counter++;
		tipo_desalojo = execute(proxima_instruccion, pcb, socket_kernel);
	} while(!check_interrupt() && tipo_desalojo == CONTINUAR_EJECUTANDO);

	ultimo_en_ejecutar = 0xFF;

	if(tipo_desalojo == CONTINUAR_EJECUTANDO) {
		desalojar_proceso(socket_kernel, pcb, DESALOJO_POR_IRQ);
		ultimo_en_ejecutar = pcb->id;
	}
}

t_instruccion *fetch(t_pcb *pcb) {
	uint32_t pc = pcb->program_counter;
	t_instruccion *instruccion = (t_instruccion *)list_get(pcb->instrucciones, pc);
	return instruccion;
}

bool decode(t_instruccion *proxima_instruccion) {
	return proxima_instruccion->identificador == COPY;
}

uint32_t fetch_operands(uint32_t tabla_primer_nivel, uint32_t direccion_logica) {
	uint32_t direccion_fisica = mmu_traducir_direccion_logica(tabla_primer_nivel, direccion_logica);
	return mmu_leer_memoria(direccion_fisica);
}

t_desalojo execute(t_instruccion *instruccion, t_pcb *pcb, int socket_kernel) {
	uint32_t direccion_fisica;
	switch(instruccion->identificador) {
		case NO_OP:
			log_info(cpu_logger, "Instruccion NO_OP ejecutada...");
			usleep(cpu_config->retardo_noop * 1000);
			break;
		case IO:
			log_info(cpu_logger, "Instruccion IO ejecutada...");
			desalojar_proceso(socket_kernel, pcb, DESALOJO_POR_IO);
			enviar_tiempo_io(socket_kernel, instruccion->primer_operando);
			return PROCESO_DESALOJADO;
		case READ:
			log_info(cpu_logger, "Instruccion READ ejecutada...");
			direccion_fisica = mmu_traducir_direccion_logica(pcb->tabla_paginas, instruccion->primer_operando);
			uint32_t valor = mmu_leer_memoria(direccion_fisica);
			log_info(cpu_logger, "Valor leido de memoria = %d", valor);
			break;
		case WRITE:
			log_info(cpu_logger, "Instruccion WRITE ejecutada...");
			direccion_fisica = mmu_traducir_direccion_logica(pcb->tabla_paginas, instruccion->primer_operando);
			mmu_escribir_memoria(direccion_fisica, instruccion->segundo_operando);
			break;
		case COPY:
			log_info(cpu_logger, "Instruccion COPY ejecutada...");
			direccion_fisica = mmu_traducir_direccion_logica(pcb->tabla_paginas, instruccion->primer_operando);
			mmu_escribir_memoria(direccion_fisica, instruccion->segundo_operando);
			break;
		case EXIT:
			log_info(cpu_logger, "Instruccion EXIT ejecutada...");
			desalojar_proceso(socket_kernel, pcb, DESALOJO_POR_EXIT);
			return PROCESO_DESALOJADO;
		default:
			break;
	}
	return CONTINUAR_EJECUTANDO;
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

void desalojar_proceso(int socket_kernel, t_pcb *pcb, t_protocolo protocolo) {
	t_paquete *paquete = serializar_pcb(pcb, protocolo);
	enviar_paquete(paquete, socket_kernel);
	eliminar_paquete(paquete);
}

void enviar_tiempo_io(int socket_kernel, uint32_t tiempo) {
	enviar_datos(socket_kernel, &tiempo, sizeof(uint32_t));
}


