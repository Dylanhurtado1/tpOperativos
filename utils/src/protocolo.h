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
	DEBUG_PAQUETE
} t_protocolo;

#endif /* PROTOCOLO_H_ */
