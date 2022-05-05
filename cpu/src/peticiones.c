#include "peticiones.h"

extern t_log *cpu_logger;
extern int socket_memoria;
extern t_cpu_config *cpu_config;
extern t_traductor *traductor;

void peticiones_dispatch(int *socket_dispatch) {
	log_info(cpu_logger, "CPU escuchando puerto dispatch");
	int socket_kernel = esperar_cliente(*socket_dispatch);
	while (true) {
		t_paquete *paquete = recibir_paquete(socket_kernel);
		switch (paquete->codigo_operacion) {
			case PCB:
				log_info(cpu_logger,"PCB recibida");
				t_pcb *pcb = deserializar_pcb(paquete, cpu_logger);

				ejecutar_ciclo_de_instruccion(pcb, socket_kernel);

				log_info(cpu_logger,"Eliminando PCB...");
				eliminar_pcb(pcb);
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

void eliminar_pcb(t_pcb *pcb) {
	list_destroy_and_destroy_elements(pcb->instrucciones, free);
	free(pcb);
}

void ejecutar_ciclo_de_instruccion(t_pcb *pcb, int socket_kernel) {
	int status;
	t_instruccion *proxima_instruccion;
	do{
		proxima_instruccion = fetch(pcb);
		bool buscar_operandos = decode(proxima_instruccion);
		int valor_a_escribir;
		if(buscar_operandos) {
			valor_a_escribir = fetch_operands(proxima_instruccion->segundo_operando, socket_memoria);
		}
		status = execute(proxima_instruccion, valor_a_escribir);
		pcb->program_counter++;
	} while(status == 0 && check_interrupt());

	if(status == 1) {
		t_paquete *paquete = serializar_pcb(pcb, BLOQUEAR_PROCESO);
		agregar_a_paquete(paquete, &(proxima_instruccion->primer_operando), sizeof(uint32_t));
		enviar_paquete(paquete, socket_kernel);
	} else if(status == 2) {
		t_paquete *paquete = serializar_pcb(pcb, FINALIZAR_PROCESO);
		enviar_paquete(paquete, socket_kernel);
	} else {
		t_paquete *paquete = serializar_pcb(pcb, PROCESO_DESALOJADO);
		enviar_paquete(paquete, socket_kernel);
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

uint32_t fetch_operands(uint32_t direccion_logica, int socket_memoria) {
	// TODO: Buscar en memoria
	return 10;
}

int execute(t_instruccion *instruccion, uint32_t valor) {
	//int direccion_logica;
	switch(instruccion->identificador) {
		case NO_OP:
			log_info(cpu_logger, "[CPU] --> Instruccion NO_OP ejecutada...");
			usleep(cpu_config->retardo_noop * 1000);
			break;
		case IO:
			log_info(cpu_logger, "[CPU] --> Instruccion IO ejecutada...");
			return 1;
		case READ:
			log_info(cpu_logger, "[CPU] --> Instruccion READ ejecutada...");
			//direccion_logica = instruccion->primer_operando;
			//exec_instruccion_READ(direccion_logica);
			break;
		case WRITE:
			log_info(cpu_logger, "[CPU] --> Instruccion WRITE ejecutada...");
			//direccion_logica=instruccion->primer_operando;
			//exec_instruccion_WRITE(direccion_logica, valor);
			break;
		case COPY:
			log_info(cpu_logger, "[CPU] --> Instruccion COPY ejecutada...");
			//direccion_logica = instruccion->primer_operando;
			//exec_instruccion_COPY(direccion_logica, valor);
			break;
		case EXIT:
			log_info(cpu_logger, "[CPU] --> Instruccion EXIT ejecutada...");
			return 2;
		default:
			break;
	}
	return 0;
}

bool check_interrupt() {
	return true;
}

void devolver_pcb(int status, int socket_kernel) {

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

int traducir_direccion(int dir_logica){
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
