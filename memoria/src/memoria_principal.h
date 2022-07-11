#ifndef MEMORIA_PRINCIPAL_H_
#define MEMORIA_PRINCIPAL_H_

#include "memoria_global.h"

typedef struct {
	uint32_t pid;
	uint8_t libre;
	uint32_t numero;
} t_marco;//cada marco tiene pid del proceso, si esta libre o no, y el numero de marco

void iniciar_memoria_principal(uint32_t tamanio_memoria, uint32_t tamanio_marco);
void eliminar_memoria_principal();
uint32_t leer_memoria_principal(uint32_t direccion);
void escribir_memoria_principal(uint32_t direccion, uint32_t valor);
void liberar_espacio_de_usuario(uint32_t pid);
void cargar_marco_en_memoria(void *buffer, uint32_t offset, uint32_t size);
void leer_marco_de_memoria(void *buffer, uint32_t offset, uint32_t size);
uint32_t cantidad_marcos_asignados(uint32_t pid);
t_marco *buscar_marco_libre();
t_marco *marco_modificado(uint32_t direccion_escrita);


#endif /* MEMORIA_PRINCIPAL_H_ */
