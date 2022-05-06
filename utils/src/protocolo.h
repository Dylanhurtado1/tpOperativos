#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include <stdint.h>

typedef struct {
	uint32_t edad;
	uint32_t peso;
	char *nombre;
} t_persona_fake;


typedef enum {
	DEBUG_MENSAJE = 1,
	DEBUG_PAQUETE,
	DATOS_CONSOLA,				// Protocolo que envia Consola a Kernel e indica el envio de instrucciones y tamanio del proceso
	FINALIZAR_CONSOLA_OK,		// Protocolo que envia Kernel a Consola e indica la finalizacion de la consola
	AGREGAR_PROCESO_A_MEMORIA,	// Protocolo que envia Kernel a Memoria para indicarle que cree las estructuras necesarias y devuelva el numero de tabla de pagina
	HANDSHAKE_INICIAL,			// Protocolo que envia CPU a Memoria e indica el envio de la configuracion para traducir direcciones logicas a fisicas
	PCB,						// Protocolo que usan Kernel, CPU y Memoria e indica el envio de la estructura PCB
	BLOQUEAR_PROCESO,			// Protocolo que envia CPU a Kernel e indica que debe bloquear al proceso
	FINALIZAR_PROCESO,			// Protocolo que envia CPU a Kernel e indica que el proceso finalizo de ejecutar sus instrucciones
	PROCESO_DESALOJADO			// Protocolo que envia CPU a Kernel e indica que el proceso fue desalojado por interrupccion (para el calculo de SJF)
	DESALOJAR_PROCESO           // Protocolo que usan Kernel y CPU para desalojar el proceso en ejecucion
} t_protocolo;

#endif /* PROTOCOLO_H_ */
