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
	FINALIZAR_CONSOLA_OK		// Protocolo que envia Kernel a Consola e indica la finalizacion de la consola
} t_protocolo;

#endif /* PROTOCOLO_H_ */
