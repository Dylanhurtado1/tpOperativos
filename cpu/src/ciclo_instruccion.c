#include "ciclo_instruccion.h"


void ejecutar_ciclo_de_instruccion(t_pcb *pcb, int socket_kernel) {//recibe el pcb y el socket de kernel para pasarle respuestas
	t_desalojo tipo_desalojo;
	t_instruccion *proxima_instruccion;

	do {
		proxima_instruccion = fetch(pcb);//fase fetch, devuelve la proxima instruccion a ejecutar y pc + 1
		if(decode(proxima_instruccion)) {//decode, busca segundo operando sólo si es COPY
			proxima_instruccion->segundo_operando = fetch_operands(pcb->tabla_paginas, proxima_instruccion->segundo_operando);
		}//se realiza el fetch operando si es COPY sólamente
		pcb->program_counter++;//el pc de pcb +1,porque va a ejecutar la proxima instruccion
		tipo_desalojo = execute(proxima_instruccion, pcb, socket_kernel);//execute
	} while(!check_interrupt() && tipo_desalojo == CONTINUAR_EJECUTANDO);//si no hay interrupciones,y el protocolo es CONTINUAR_EJECUTANDO
//vuelve a hacer el ciclo de instruccion
	if(tipo_desalojo == CONTINUAR_EJECUTANDO) {//esta parte preguntar, esto esta bien? no seria PROCESO_DESALOJADO??
		desalojar_proceso(socket_kernel, pcb, DESALOJO_POR_IRQ);//envio pcb a kernel, con protocolo DESALOJO_POR_IRQ, se envia por interrupciones
	}
}

t_instruccion *fetch(t_pcb *pcb) {//devuelve una instruccion de la lista de instrucciones a realizar
	uint32_t pc = pcb->program_counter;
	t_instruccion *instruccion = (t_instruccion *)list_get(pcb->instrucciones, pc);
	return instruccion;//solo devuelve una instruccion de toda la lista
}

bool decode(t_instruccion *proxima_instruccion) {
	return proxima_instruccion->identificador == COPY;//solo devuelve un booleano si es COPY
}

uint32_t fetch_operands(uint32_t tabla_primer_nivel, uint32_t direccion_logica) {//importante, dir logica y fisica son numeros
	uint32_t direccion_fisica = mmu_traducir_direccion_logica(tabla_primer_nivel, direccion_logica);//recibe el numero de la tabla de primer nivel, y la dir logica
	return mmu_leer_memoria(direccion_fisica);//devuelve un numero recibido por memoria
}

t_desalojo execute(t_instruccion *instruccion, t_pcb *pcb, int socket_kernel) {//aca cpu realiza la ejecucion de las instrucciones
	uint32_t direccion_fisica;
	switch(instruccion->identificador) {//de la instruccion busca el identificador
		case NO_OP:
			log_info(cpu_logger, "Instruccion NO_OP ejecutada...");
			usleep(cpu_config->retardo_noop * 1000);//se queda esperando un cierto tiempo
			break;
		case IO://tiene un numero
			log_info(cpu_logger, "Instruccion IO ejecutada...");//
			desalojar_proceso(socket_kernel, pcb, DESALOJO_POR_IO);//envio el pcb a kernel, con el protocolo, kernel hace el calculo
			enviar_tiempo_io(socket_kernel, instruccion->primer_operando);//envio el tiempo que tiene q estar bloqueado a kernel tambien
			return PROCESO_DESALOJADO;//esta respuesta a quien?
		case READ://se lee el valor del primer operando en memoria y debo imprimirlo
			log_info(cpu_logger, "Instruccion READ ejecutada...");
			direccion_fisica = mmu_traducir_direccion_logica(pcb->tabla_paginas, instruccion->primer_operando);//traduzo a dir fisica
			uint32_t valor = mmu_leer_memoria(direccion_fisica);//me devuelve un valor memoria
			log_info(cpu_logger, "Valor leido de memoria = %d", valor);//ese valor con protocolo LEER, lo imprimo
			break;
		case WRITE://(dir logica,valor) se escribe en la dir logica, valor
			log_info(cpu_logger, "Instruccion WRITE ejecutada...");
			direccion_fisica = mmu_traducir_direccion_logica(pcb->tabla_paginas, instruccion->primer_operando);//obtengo dir fisica
			mmu_escribir_memoria(direccion_fisica, instruccion->segundo_operando);//no retorno nada a cpu, hace todo en memoria
			break;
		case COPY://(dir logica destino,dir logica origen)
			log_info(cpu_logger, "Instruccion COPY ejecutada...");//
			direccion_fisica = mmu_traducir_direccion_logica(pcb->tabla_paginas, instruccion->primer_operando);//obtengo la dir fisica con el primer operando
			mmu_escribir_memoria(direccion_fisica, instruccion->segundo_operando);//tengo que escrbir en memoria el 2do operando, pero primero tengo q obtener la dir fisica
			break;//no hace falta traducir la dir logica origen, porque para copiar se tuvo que hacer escritooo
		case EXIT://se devuelve a kernel
			log_info(cpu_logger, "Instruccion EXIT ejecutada...");
			desalojar_proceso(socket_kernel, pcb, DESALOJO_POR_EXIT);//lo mando por socket a kernel el pcb, con protocolo
			return PROCESO_DESALOJADO;
		default:
			break;
	}
	return CONTINUAR_EJECUTANDO;
}

bool check_interrupt() {
	pthread_mutex_lock(&mutex_interrupt);
	bool status = interrupcion_desalojo;
	if(interrupcion_desalojo) {//si hay interrupciones
		interrupcion_desalojo = false;//lo cambio a false, pero retorna un booleano indicando que hay interrupciones
	}
	pthread_mutex_unlock(&mutex_interrupt);

	return status;//retorna un booleano que dice si hay interrupcion o no
}

void desalojar_proceso(int socket_kernel, t_pcb *pcb, t_protocolo protocolo) {//envio a kernel el pcb actualizado con protocolo para que kernel entienda que hacer
	t_paquete *paquete = serializar_pcb(pcb, protocolo);
	enviar_paquete(paquete, socket_kernel);
	eliminar_paquete(paquete);
}

void enviar_tiempo_io(int socket_kernel, uint32_t tiempo) {//envio datos por socket
	enviar_datos(socket_kernel, &tiempo, sizeof(uint32_t));
}


