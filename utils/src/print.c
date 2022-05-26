#include "print.h"

void print_datos_consola(t_log *logger, t_list *instrucciones, uint32_t tamanio) {
	log_info(logger, "************ DATOS CONSOLA **********");
	log_info(logger,"Cantidad de instrucciones recibidas = %d", list_size(instrucciones));
	//print_instrucciones(logger, instrucciones);
	log_info(logger,"Tamanio de consola = %d", tamanio);
	log_info(logger, "*************************************");
}

void print_instrucciones(t_log *logger, t_list *instrucciones) {
	log_info(logger, "************ INTRUCCIONES ***********");
	for(int i = 0; i < list_size(instrucciones); i++) {
		t_instruccion *instruccion = (t_instruccion *)list_get(instrucciones, i);
		log_info(logger, "ID: %d, OP_1: %d, OP_2: %d",
				instruccion->identificador, instruccion->primer_operando, instruccion->segundo_operando);
	}
	log_info(logger, "*************************************");
}
