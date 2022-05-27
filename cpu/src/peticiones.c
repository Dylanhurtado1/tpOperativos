#include "peticiones.h"


void peticiones_dispatch(int *socket_dispatch) {
	t_pcb *pcb;

	log_info(cpu_logger, "CPU escuchando puerto dispatch");
	int socket_kernel = esperar_cliente(*socket_dispatch);
	while (true) {
		t_paquete *paquete = recibir_paquete(socket_kernel);
		switch (paquete->codigo_operacion) {
			case PCB:
				pcb = deserializar_pcb(paquete);
				log_info(cpu_logger,"PCB[%d] recibido, ejecutando instrucciones", pcb->id);

				ejecutar_ciclo_de_instruccion(pcb, socket_kernel);

				eliminar_pcb(pcb);
				break;
			default:
				log_error(cpu_logger, "Protocolo invalido.");
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
				log_info(cpu_logger, "Interrupcion recibida, se desalojara proceso...");
				pthread_mutex_lock(&mutex_interrupt);
				interrupcion_desalojo = true;
				pthread_mutex_unlock(&mutex_interrupt);
				break;
			default:
				log_error(cpu_logger, "Protocolo invalido.");
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
