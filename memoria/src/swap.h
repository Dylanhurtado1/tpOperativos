#ifndef SWAP_H_
#define SWAP_H_

#include "memoria_global.h"
#include "memoria_principal.h"

typedef struct {
	uint32_t pid;
	char *path;
	FILE *fd;
} t_swap;

void swap_crear_archivo(uint32_t pid, uint32_t tamanio_archivo);
void swap_eliminar_archivo(uint32_t pid);
void swap_leer_pagina(uint32_t pid, void *buffer, uint32_t offset, uint32_t size);
void swap_escribir_pagina(uint32_t pid, void *buffer, uint32_t offset, uint32_t size);
void swap_out(uint32_t pid, uint32_t pagina, uint32_t marco);
void swap_in(uint32_t pid, uint32_t pagina, uint32_t marco);


#endif /* SWAP_H_ */
