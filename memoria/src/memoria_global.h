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


#endif /* MEMORIA_GLOBAL_H_ */
