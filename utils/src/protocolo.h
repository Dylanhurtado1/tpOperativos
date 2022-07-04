#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include <stdint.h>

typedef enum {
	DEBUG_MENSAJE = 1,
	DEBUG_PAQUETE,

	DATOS_CONSOLA,					// Protocolo que envia Consola a Kernel para indicar el envio de instrucciones y tamanio del proceso
	FINALIZAR_CONSOLA,				// Protocolo que envia Kernel a Consola para indicar la finalizacion de la consola

	INICIALIZACION_DE_PROCESO,		// Protocolo que envia Kernel a Memoria para indicarle que cree las estructuras necesarias y devuelva el numero de tabla de pagina
	SUSPENSION_DE_PROCESO,			// Protocolo que envia Kernel a Memoria para indicar que se debe liberar el espacio de memoria del procesos y escribir en SWAP
	FINALIZACION_DE_PROCESO,		// Protocolo que envia Kernel a Memoria para indicar que se debe liberar la memoria y eliminar el archivo SWAP
	PROCESO_SUSPENDIDO,				// Protocolo que envia Memoria a Kernel para indicar que se libero la memoria del proceso
	PROCESO_FINALIZADO,				// Protocolo que envia Memoria a Kernel para indicar que se libero la memoria y se elimino el archivo SWAP

	PCB,							// Protocolo que usan Kernel, CPU y Memoria e indica el envio de la estructura PCB

	DESALOJAR_PROCESO,          	// Protocolo que envia Kernel a CPU para indicar que desaloje el proceso en ejecucion
	DESALOJO_POR_IO,				// Protocolo que envia CPU a Kernel para indicar que el proceso fue desalojado por ejecutar IO
	DESALOJO_POR_EXIT,				// Protocolo que envia CPU a Kernel para indicar que el proceso fue desalojado por ejecutar EXIT
	DESALOJO_POR_IRQ,				// Protocolo que envia CPU a Kernel para indicar que el proceso fue desalojado por interrupccion (para el calculo de SJF)

	HANDSHAKE_INICIAL,				// Protocolo que envia CPU a Memoria e indica el envio de la configuracion para traducir direcciones logicas a fisicas
	ACCESO_TABLA_PRIMER_NIVEL,		// Protocolo que envia CPU/MMU a Memoria para pedir la direccion de la tabla de segundo nivel
	ACCESO_TABLA_SEGUNDO_NIVEL,		// Protocolo que envia CPU/MMU a Memoria para pedir el marco de la pagina en la tabla de segundo nivel
	LEER_MEMORIA,					// Protocolo que envia CPU/MMU a Memoria para pedir el valor almacenado en memoria
	ESCRIBIR_MEMORIA				// Protocolo que envia CPU/MMU a Memoria escribir en memoria un valor
} t_protocolo;

#endif /* PROTOCOLO_H_ */
