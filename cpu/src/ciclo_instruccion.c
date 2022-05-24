#include "ciclo_instruccion.h"

extern t_log *cpu_logger;
extern int socket_memoria;
extern t_cpu_config *cpu_config;
extern t_traductor *traductor;
extern sem_t sem_interrupt;
extern bool desalojar_proceso;

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
	} while(status == 0 && !check_interrupt());

	t_paquete *paquete;
	if(status == 1) {
		pcb->tiempo_io = proxima_instruccion->primer_operando;
		paquete = serializar_pcb(pcb, BLOQUEAR_PROCESO);
	} else if(status == 2) {
		paquete = serializar_pcb(pcb, FINALIZAR_PROCESO);
	} else {
		paquete = serializar_pcb(pcb, PROCESO_DESALOJADO);
	}
	enviar_paquete(paquete, socket_kernel);
	eliminar_paquete(paquete);
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
	sem_wait(&sem_interrupt);
	bool status = desalojar_proceso;
	if(desalojar_proceso) {
		desalojar_proceso = false;
	}
	sem_post(&sem_interrupt);

	return status;
}


