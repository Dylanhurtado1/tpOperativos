#include "peticiones.h"

extern t_log *cpu_logger;
extern int socket_memoria;
//extern t_cpu_config *config;

void peticiones_dispatch(int *socket_dispatch) {
	log_info(cpu_logger, "CPU escuchando puerto dispatch");
	int socket_kernel = esperar_cliente(*socket_dispatch);
	while (true) {
		t_paquete *paquete = recibir_paquete(socket_kernel);
		switch (paquete->codigo_operacion) {
			case PCB:
				log_info(cpu_logger,"PCB recibida");
				t_pcb *pcb = deserializar_pcb(paquete, cpu_logger);

				ejecutar_ciclo_de_instruccion(pcb);

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

void ejecutar_ciclo_de_instruccion(t_pcb *pcb) {

	int tamanio_list_instrucciones = list_size(pcb->instrucciones);
	do{
		t_instruccion *proxima_instruccion = fetch(pcb);
		bool buscar_operandos = decode(proxima_instruccion);
		int valor_a_escribir;
		if(buscar_operandos) {
			valor_a_escribir = fetch_operands(proxima_instruccion->segundo_operando, socket_memoria);
		}
		if(execute(proxima_instruccion, valor_a_escribir)==0){
			pcb->program_counter++;
		} else  if(execute(proxima_instruccion, valor_a_escribir)==1) {
			// IO
			//tomar el primer operando de la instruccion y guardarlo en una var de tiempo block
			pcb->program_counter++;
			// serializar la pcb junto al tiempo block
			// enviar el paquete a kernel para que lo bloquee
		} else  if(execute(proxima_instruccion, valor_a_escribir)==2){
			// EXIT
			pcb->program_counter++;
			// SERIALIZAR EL PCB NORMAL Y ENVIAR A KERNEL

		}
	} while(pcb->program_counter<=tamanio_list_instrucciones );
}

t_instruccion *fetch(t_pcb *pcb) {
	uint32_t pc = pcb->program_counter;
	t_instruccion *instruccion = list_get(pcb->instrucciones, pc);
	return instruccion;
}

bool decode(t_instruccion *proxima_instruccion) {
	return proxima_instruccion->identificador == COPY;
}

uint32_t fetch_operands(uint32_t operando, int socket_fd) {
	// TODO: Buscar en memoria
	return 10;
}

int execute(t_instruccion *instruccion, uint32_t valor) {
	int i,ciclosCPU, ciclosIO;
	int dir_logica;
	switch(instruccion->identificador) {
		case NO_OP: // Cada instruccion NO_OP corresponde a 1 ciclo de CPU
				log_info(cpu_logger, "Se ejecuto operacion NO_OP %d", i+1);
				usleep(1000); //deberia leer RETARDO_NOOP del config de cpu
			break;
		case IO:
			// ACA DEBE MANDAR A KERNEL LA PCB PARA QUE LO BLOQUEE mediante socket
			// le enviamos la pcb y el tiempo de ciclos IO Por la cual debe blockearse
			return 1;
		case READ:
            dir_logica=instruccion->primer_operando;
			exec_instruccion_READ (dir_logica);
			break;
		case WRITE:
			dir_logica=instruccion->primer_operando;
			exec_instruccion_WRITE (dir_logica, valor);
			break;
		case COPY:
			dir_logica=instruccion->primer_operando;
			exec_instruccion_COPY (dir_logica, valor);
			break;
		case EXIT:
			// enviar al Kernel el PCB actualizado y msj finalizado
			return 2;
		default:
			break;
	}
	return 0;
}
void exec_instruccion_READ (int dir_logica){
	int dir_fisica =traducir_direccion(dir_logica);

}
void exec_instruccion_WRITE (int dir_logica, int valor){
	// traduccion y acceso a memoria
}
void exec_instruccion_COPY (int dir_logica_destino, int dir_logica_origen){

}

int traducir_direccion(int dir_logica){
	// ACA SE IMPLEMENTARIA EL PROCESO DE TRADUCCION DE DIR LOGICA A FISICA
	/*
	int numero_pagina = dir_logica/tamanio_pagina;
	int entrada_tabla_1er_nivel = numero_pagina/cantidad_entradas_por_tabla;
	int entrada_tabla_2do_nivel = numero_pagina mod (cantidad_entradas_por_tabla);
	int desplazamiento = dir_logica-numero_pagina*tamanio_pagina;

	// CREAR ESTRUCTURA TLB en otra funcion posiblemente
*/
	return 0;
}
