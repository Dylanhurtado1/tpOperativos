#include "memoria_principal.h"

static void crear_marcos_memoria(uint32_t cantidad_marcos);
static bool marco_libre(t_marco *marco);


void iniciar_memoria_principal(uint32_t tamanio_memoria, uint32_t tamanio_marco) {//La memoria dividida en marcos
	memoria_principal = malloc(tamanio_memoria);
	memset(memoria_principal, 0, tamanio_memoria);

	crear_marcos_memoria(tamanio_memoria / tamanio_marco);//crear_marcos_memoria(cant de marcos),tam paginas==tam marco?
}

void eliminar_memoria_principal() {
	free(memoria_principal);
}

uint32_t leer_memoria_principal(uint32_t direccion) {
	uint32_t valor;
	memcpy(&valor, memoria_principal + direccion, sizeof(uint32_t));//se copia en valor, mem principal + dir fisica, porque?
	return valor;
}

void escribir_memoria_principal(uint32_t direccion, uint32_t valor) {//Se copia el valor, en la mem princial, es inverso al LEER, Atento con eso
	memcpy(memoria_principal + direccion, &valor, sizeof(uint32_t));
}

void liberar_espacio_de_usuario(uint32_t pid) {//poner marcos libres en el bitmap, porque fue suspendido
	bool marco_id(t_marco *marco) {
		return marco->pid == pid;
	}

	void liberar_marco(t_marco *marco) {//el marco q recibe como parametro lo pone como libre, porque libera espacio de usuario
		marco->pid = 0xFF;
		marco->libre = 1;
	}

	t_list *marcos_asignados = list_filter(bitmap_marcos, (void *)marco_id);//retorna una lista de marcos que tengas ese pid
	list_iterate(bitmap_marcos, (void *)liberar_marco);//aplica esa funcion a toda la lista

	list_destroy(marcos_asignados);//y elimina esa lista, porque ya no me sirve, sólo para aplicar cambios al bit map
}

void cargar_marco_en_memoria(void *buffer, uint32_t offset, uint32_t size) {
	memcpy(memoria_principal + offset, buffer, size);
}

void leer_marco_de_memoria(void *buffer, uint32_t offset, uint32_t size) {//se copia en el buffer, mem+offset, porque?
	memcpy(buffer, memoria_principal + offset, size);
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

t_marco *marco_modificado(uint32_t direccion_escrita) {//con la dir fisica
	uint32_t numero_marco = (uint32_t)(direccion_escrita / memoria_config->tamanio_pagina);//obtengo el numero de marco
	return list_get(bitmap_marcos, numero_marco);//y saco ese marco del bit map para modificarlo
}


static void crear_marcos_memoria(uint32_t cantidad_marcos) {
	bitmap_marcos = list_create();//para saber los marcos libres
	for(int i = 0; i < cantidad_marcos; i++) {
		t_marco *marco = malloc(sizeof(t_marco));
		marco->pid = 0xFF;
		marco->libre = 1;
		marco->numero = i;
		list_add(bitmap_marcos, marco);//se agrega un marco a la lista de marcos
	}
}

static bool marco_libre(t_marco *marco) {
	return marco->libre == 1;
}

