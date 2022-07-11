#include "peticiones.h"


void peticiones_dispatch(int *socket_dispatch) {//es un servidor
	t_pcb *pcb;
	log_info(cpu_logger, "CPU escuchando puerto dispatch");
	int socket_kernel = esperar_cliente(*socket_dispatch);

	while (true) {
		t_paquete *paquete = recibir_paquete(socket_kernel);//espera request de kernel
		switch (paquete->codigo_operacion) {
			case PCB://EJECUTA LAS INSTRUCCIONES DEL Proceso
				pcb = deserializar_pcb(paquete);
				log_info(cpu_logger,"PCB[%d] recibido, ejecutando instrucciones", pcb->id);

				ejecutar_ciclo_de_instruccion(pcb, socket_kernel);//ciclo de instruccion,fetch,decode,operand,execute, y check interrupts
				tlb_limpiar_cache();

				eliminar_pcb(pcb);
				break;
			default:
				log_error(cpu_logger, "Protocolo invalido, finalizando CPU");
				abort();
				break;
		}
		eliminar_paquete(paquete);
	}
}

void peticiones_interrupt(int *socket_interrupt) {//servidor a la escucha
	t_protocolo protocolo;
	log_info(cpu_logger, "CPU escuchando puerto interrupt");
	int socket_kernel = esperar_cliente(*socket_interrupt);

	while (true) {
		recibir_datos(socket_kernel, &protocolo, sizeof(t_protocolo));//espera protocolo de kernel
		switch (protocolo) {
			case DESALOJAR_PROCESO://protocolo
				log_info(cpu_logger, "Interrupcion recibida, se desalojara proceso...");//pero solo se imprime por pantalla
				pthread_mutex_lock(&mutex_interrupt);
				interrupcion_desalojo = true;//esta variable entra en juego
				pthread_mutex_unlock(&mutex_interrupt);
				break;
			default:
				log_error(cpu_logger, "Protocolo invalido, finalizando CPU");
				abort();
				break;
		}
	}
}

