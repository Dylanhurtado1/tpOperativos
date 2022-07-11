#include "gestionar_consola.h"


void procesar_datos_consola(t_cliente *datos_cliente) {
	t_paquete *paquete = datos_cliente->paquete;

	switch (paquete->codigo_operacion) {
		case DATOS_CONSOLA://protocolo que consola envia a kernel
			log_info(kernel_logger, "Procesando datos de consola");
			t_consola *consola = deserializar_consola(paquete);

			t_proceso *proceso = crear_proceso(consola, datos_cliente->socket);//planificador de largo plazo lo crea
			agregar_proceso_a_new(proceso);//se agrega a la cola new

			eliminar_consola(consola);//y se elimina toda la consola, lista de instrucciones y consola
			break;
		default:
			log_error(kernel_logger,"Protocolo invalido.");
			break;
	}
}
