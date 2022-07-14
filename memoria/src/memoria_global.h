#ifndef MEMORIA_GLOBAL_H_
#define MEMORIA_GLOBAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <protocolo.h>
#include <conexiones.h>
#include <estructuras.h>
#include <serializador.h>
#include "memoria_config.h"

t_log *memoria_logger;
t_memoria_config *memoria_config;
void *memoria_principal;
t_list *tablas_de_paginacion;
t_list *bitmap_marcos;
t_list *archivos_swap;
t_list *punteros_clock;
pthread_mutex_t mutex_swap;
pthread_mutex_t mutex_memoria;
uint32_t cantidad_acceso_disco;
uint32_t cantidad_page_fault;


#endif /* MEMORIA_GLOBAL_H_ */
