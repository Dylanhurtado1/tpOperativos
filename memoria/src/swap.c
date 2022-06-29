#include "swap.h"

static void agregar_swap(uint32_t pid, char *path, FILE *fd);
static t_swap *buscar_swap(uint32_t pid);


void swap_crear_archivo(uint32_t pid, uint32_t tamanio_archivo) {
	char *path = string_from_format("%s/%d.swap", memoria_config->path_swap, pid);
	FILE *fd = fopen(path, "w+b");

	if(fd == NULL) {
		log_error(memoria_logger, "No se pudo crear el archivo swap del PID[%d]", pid);
		abort();
	}

	uint32_t dato_a_llenar = 0;
	uint32_t tamanio_u32 = tamanio_archivo / sizeof(uint32_t);
	for(int i = 0; i < tamanio_u32; i++) {
		fwrite(&dato_a_llenar, sizeof(uint32_t), 1, fd);
	}

	agregar_swap(pid, path, fd);
}

void swap_eliminar_archivo(uint32_t pid) {
	bool swap_id(t_swap *swap) {
		return swap->pid == pid;
	}
	t_swap *swap = list_remove_by_condition(archivos_swap, (void *)swap_id);

	fclose(swap->fd);
	remove(swap->path);

	free(swap->path);
	free(swap);
}

void swap_leer_pagina(uint32_t pid, void *buffer, uint32_t offset, uint32_t size) {
	t_swap *swap = buscar_swap(pid);

	fseek(swap->fd, offset, SEEK_SET);
	fread(buffer, size, 1, swap->fd);
}

void swap_escribir_pagina(uint32_t pid, void *buffer, uint32_t offset, uint32_t size) {
	t_swap *swap = buscar_swap(pid);

	fseek(swap->fd, offset, SEEK_SET);
	fwrite(buffer, size, 1, swap->fd);
}

void swap_out(uint32_t pid, uint32_t pagina, uint32_t marco) {
	void *buffer = malloc(memoria_config->tamanio_pagina);
	leer_marco_de_memoria(buffer, marco * memoria_config->tamanio_pagina, memoria_config->tamanio_pagina);
	swap_escribir_pagina(pid, buffer, pagina * memoria_config->tamanio_pagina, memoria_config->tamanio_pagina);
	free(buffer);
}

void swap_in(uint32_t pid, uint32_t pagina, uint32_t marco) {
	void *buffer = malloc(memoria_config->tamanio_pagina);
	swap_leer_pagina(pid, buffer, pagina * memoria_config->tamanio_pagina, memoria_config->tamanio_pagina);
	cargar_marco_en_memoria(buffer, marco * memoria_config->tamanio_pagina, memoria_config->tamanio_pagina);
	free(buffer);
}


static void agregar_swap(uint32_t pid, char *path, FILE *fd) {
	t_swap *swap = malloc(sizeof(t_swap));
	swap->pid = pid;
	swap->path = path;
	swap->fd = fd;
	list_add(archivos_swap, swap);
}

static t_swap *buscar_swap(uint32_t pid) {
	bool swap_id(t_swap *swap) {
		return swap->pid == pid;
	}
	return list_find(archivos_swap, (void *)swap_id);
}
