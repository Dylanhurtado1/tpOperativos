#include "peticiones.h"

extern t_log *cpu_logger;
extern sem_t sem_interrupt;
extern bool desalojar_proceso;


void peticiones_dispatch(int *socket_dispatch) {
	log_info(cpu_logger, "CPU escuchando puerto dispatch");
	int socket_kernel = esperar_cliente(*socket_dispatch);
	while (true) {
		t_paquete *paquete = recibir_paquete(socket_kernel);
		switch (paquete->codigo_operacion) {
			case PCB:
				log_info(cpu_logger,"PCB recibida");
				t_list *datos = deserealizar_paquete(paquete);
				t_pcb *pcb = deserializar_pcb(datos, cpu_logger);
				list_destroy_and_destroy_elements(datos, free);

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
			case DESALOJAR_PROCESO:
				log_info(cpu_logger, "Interrupcion recibida, proximamente se desalojara el proceso...");
				sem_wait(&sem_interrupt);
				desalojar_proceso = true;
				sem_post(&sem_interrupt);
				break;
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
