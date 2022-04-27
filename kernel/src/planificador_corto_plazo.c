#include "planificador.h"

void eliminar_proceso_cola_ready(t_pcb *proceso);
void eliminar_proceso(t_pcb *proceso);
//void enviar_interrupcion_a_cpu(int socket_fd);

t_queue *cola_ready;
t_queue *cola_new;
bool exec = false;

//extern int socket_cpu_interrupt;
extern int socket_cpu_dispatch;
extern t_log *kernel_logger;
extern uint32_t procesos_admitidos_en_ready;

void agregar_proceso_a_ready(t_pcb *proceso) {
	queue_push(cola_ready, proceso);
}

void ejecutar_proceso() {
	if(hay_proceso_en_ejecucion()) {
		log_info(kernel_logger, "Hay un proceso en ejecucion");
		return;
	}
	t_pcb *proceso = queue_pop(cola_ready);
	t_paquete *paquete = serealizar_proceso(proceso);
	enviar_paquete(paquete, socket_cpu_dispatch);
	eliminar_paquete(paquete);
	eliminar_proceso(proceso);
	procesos_admitidos_en_ready--;

}

bool hay_proceso_en_ejecucion() {
	return exec;
}

void iniciar_cola_ready() {
	cola_ready = queue_create();
}

void eliminar_cola_ready() {
	queue_destroy_and_destroy_elements(cola_ready, (void *)eliminar_proceso_cola_ready);
}

void eliminar_proceso_cola_ready(t_pcb *proceso) {
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}

void eliminar_proceso(t_pcb *proceso) {
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}

t_paquete *serealizar_proceso(t_pcb *proceso) {
	t_paquete *paquete = crear_paquete(PCB, buffer_vacio());
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



bool hay_procesos_en_ready(){
	int cantidadReady = queue_size(cola_ready);
	return cantidadReady > 0;
}

/*
void agregar_proceso_a_ready_SRT(t_pcb *proceso){
	proceso = queue_pop(cola_new);
	queue_push(cola_ready, proceso);
	enviar_interrupcion_a_cpu(socket_cpu_interrupt);
	procesos_admitidos_en_ready++;
}
*/

/*
void enviar_interrupcion_a_cpu(int socket_fd){
	uint32_t nada = 0;
	t_pcb *pcbProceso;
	t_paquete *paquete = crear_paquete(DESALOJAR_PROCESO, buffer_vacio());
	agregar_a_paquete(paquete, &nada, sizeof(uint32_t));           //Envia una interrupcion, igual que con memoria no tiene que enviar datos
	enviar_paquete(paquete, socket_fd);
	eliminar_paquete(paquete);

	//recibir_datos(socket_fd, &pcbProceso, sizeof(t_pcb));
	//log_info(kernel_logger, "PCB del proceso en ejecucion desalojado recibido");

}
*/

void planificacionFIFO(){//ESTA ES UNA IMPLEMENTACION CON LISTAS, HAY QUE DEFINIR COLAS O LISTAS, PERO ES
	//UN APROXIMADO A LO QUE PENSE, NO ESTA TERMINADO
	    int tamanioReady = 5;//SUPONIENDO HAY 5 procesos en ready, nose si estoy haciendolo bien xD
		int tamanioNew;
		int grado_multitarea = 2;//cantidad de procesos que pueden estar ejecutando
		int ejecutando = 1;
		t_list* procesosReady;
		t_list* procesosExec;
	log_info(kernel_logger,"Hilo de planificador FIFO iniciado");

	while(true){
		//if(hay_procesos_en_ready() && !hayUnProcesoEnEjecucion()){
		//	ejecutar_proceso();
		//el pequeño fifo xD
			if (tamanioReady > 0){ //si hay procesos en ready
			if (ejecutando<grado_multitarea){//cant de procesos ejecutando < grado que permite
			int i=0;//y con este codigo todos los tripulantes de rdy pasar a exec
			while(ejecutando<grado_multitarea && (tamanioReady-i)>0){//cant de procesos en rdy > 0
			t_pcb* primerProceso= list_get(procesosReady,0);
			list_remove(procesosReady,0);
			list_add(procesosExec,primerProceso);//POR AHORA EL PANIF ES EL UNICO QUE MANEJA ESTA LISTA
			i++;
			ejecutando++;
			log_info(kernel_logger,"Se pasaron algunos procesos de ready a Exec");
			}
			}
			}
		}
}



void planificacionSRT(){
	log_info(kernel_logger,"Hilo de planificador SRT iniciado");

	while(true){
		//if(hay_procesos_en_ready()){
			//ejecutar_proceso();
		}
	}
























