#include "memoria.h"


void init() {
	memoria_logger = log_create("memoria.log", "MEMORIA", true, LOG_LEVEL_INFO);
	memoria_config = memoria_leer_configuracion(PATH_MEMORIA_CONFIG);
	iniciar_memoria_principal(memoria_config->tamanio_memoria, memoria_config->tamanio_pagina);//creo la memoria con la config,memoria dividida en frames
	tablas_de_paginacion = list_create();//es una lista
	archivos_swap = list_create();//es una lista
	punteros_clock = list_create();//otra lista
	pthread_mutex_init(&mutex_swap, NULL);
}

int main(void) {

	init();
    //memoria como servidor, escuchando
	int socket_memoria = iniciar_modulo_servidor(memoria_config->ip_memoria, memoria_config->puerto_escucha, memoria_logger);
	log_info(memoria_logger, "Memoria iniciado como servidor");

	if(atender_clientes(socket_memoria, procesar_conexiones) == WAIT_CLIENT_ERROR) {//se queda escuchando a los clientes
		log_error(memoria_logger, "Error al escuchar clientes... Finalizando servidor");
	}

	log_destroy(memoria_logger);
	free(memoria_principal);
	memoria_eliminar_configuracion(memoria_config);
	cerrar_conexion(socket_memoria);

	return EXIT_SUCCESS;
}

