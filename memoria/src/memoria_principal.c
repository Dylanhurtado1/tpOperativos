#include "memoria_principal.h"

static void crear_marcos_memoria(uint32_t cantidad_marcos);
static bool marco_libre(t_marco *marco);


void iniciar_memoria_principal(uint32_t tamanio_memoria, uint32_t tamanio_marco) {
	memoria_principal = malloc(tamanio_memoria);
	memset(memoria_principal, 0, tamanio_memoria);

	crear_marcos_memoria(tamanio_memoria / tamanio_marco);
}

void eliminar_memoria_principal() {
	free(memoria_principal);
}

uint32_t leer_memoria_principal(uint32_t direccion) {
	uint32_t valor;
	pthread_mutex_lock(&mutex_memoria);
	memcpy(&valor, memoria_principal + direccion, sizeof(uint32_t));
	pthread_mutex_unlock(&mutex_memoria);
	return valor;
}

void escribir_memoria_principal(uint32_t direccion, uint32_t valor) {
	pthread_mutex_lock(&mutex_memoria);
	memcpy(memoria_principal + direccion, &valor, sizeof(uint32_t));
	pthread_mutex_unlock(&mutex_memoria);
}

void liberar_espacio_de_usuario(uint32_t pid) {
	bool marco_id(t_marco *marco) {
		return marco->pid == pid;
	}

	void liberar_marco(t_marco *marco) {
		marco->pid = 0xFF;
		marco->libre = 1;
	}

	t_list *marcos_asignados = list_filter(bitmap_marcos, (void *)marco_id);
	if(!list_is_empty(marcos_asignados)) {
		list_iterate(marcos_asignados, (void *)liberar_marco);
	}

	list_destroy(marcos_asignados);
}

void cargar_marco_en_memoria(void *buffer, uint32_t offset, uint32_t size) {
	pthread_mutex_lock(&mutex_memoria);
	memcpy(memoria_principal + offset, buffer, size);
	pthread_mutex_unlock(&mutex_memoria);
}

void leer_marco_de_memoria(void *buffer, uint32_t offset, uint32_t size) {
	pthread_mutex_lock(&mutex_memoria);
	memcpy(buffer, memoria_principal + offset, size);
	pthread_mutex_unlock(&mutex_memoria);
}

uint32_t cantidad_marcos_asignados(uint32_t pid) {
	bool marco_id(t_marco *marco) {
		return marco->pid == pid;
	}
	t_list *marcos_asignados = list_filter(bitmap_marcos, (void *)marco_id);
	uint32_t cantidad_marcos = list_size(marcos_asignados);
	list_destroy(marcos_asignados);

	return cantidad_marcos;
}

t_marco *buscar_marco_libre() {
	return list_find(bitmap_marcos, (void *)marco_libre);
}

t_marco *marco_modificado(uint32_t direccion_escrita) {
	uint32_t numero_marco = (uint32_t)(direccion_escrita / memoria_config->tamanio_pagina);
	return list_get(bitmap_marcos, numero_marco);
}


static void crear_marcos_memoria(uint32_t cantidad_marcos) {
	bitmap_marcos = list_create();
	for(int i = 0; i < cantidad_marcos; i++) {
		t_marco *marco = malloc(sizeof(t_marco));
		marco->pid = 0xFF;
		marco->libre = 1;
		marco->numero = i;
		list_add(bitmap_marcos, marco);
	}
}

static bool marco_libre(t_marco *marco) {
	return marco->libre == 1;
}

