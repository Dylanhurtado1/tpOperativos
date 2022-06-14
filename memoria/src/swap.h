#ifndef SWAP_H_
#define SWAP_H_

#include "memoria_global.h"

typedef struct {
	uint32_t pid;
	char *path;
	FILE *fd;
} t_swap;

void swap_crear_archivo(uint32_t pid, uint32_t tamanio_archivo);
void swap_eliminar_archivo(uint32_t pid);
void swap_leer_pagina(uint32_t pid, void *buffer, uint32_t offset, uint32_t size);
void swap_escribir_pagina(uint32_t pid, void *buffer, uint32_t offset, uint32_t size);


#endif /* SWAP_H_ */
