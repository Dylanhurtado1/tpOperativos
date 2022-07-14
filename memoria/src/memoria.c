#include "memoria.h"


void init(char *config_path) {
	//memoria_logger = log_create("memoria.log", "MEMORIA", true, LOG_LEVEL_INFO);
	memoria_config = memoria_leer_configuracion(config_path);
	iniciar_memoria_principal(memoria_config->tamanio_memoria, memoria_config->tamanio_pagina);
	tablas_de_paginacion = list_create();
	archivos_swap = list_create();
	punteros_clock = list_create();
	pthread_mutex_init(&mutex_swap, NULL);
	pthread_mutex_init(&mutex_memoria, NULL);
	cantidad_acceso_disco = 0;
	cantidad_page_fault = 0;
}

int main(int argc, char **argv) {
	memoria_logger = log_create("memoria.log", "MEMORIA", true, LOG_LEVEL_INFO);
	if(argc < 2) {
		log_error(memoria_logger, "Error de parametros. Ejemplo de uso: ./memoria <archivo_configuracion>");
		log_destroy(memoria_logger);
		return EXIT_FAILURE;
	}
	init(argv[1]);

	int socket_memoria = iniciar_modulo_servidor(memoria_config->ip_memoria, memoria_config->puerto_escucha, memoria_logger);
	log_info(memoria_logger, "Memoria iniciado como servidor");

	if(atender_clientes(socket_memoria, procesar_conexiones) == WAIT_CLIENT_ERROR) {
		log_error(memoria_logger, "Error al escuchar clientes... Finalizando servidor");
	}

	log_destroy(memoria_logger);
	free(memoria_principal);
	memoria_eliminar_configuracion(memoria_config);
	cerrar_conexion(socket_memoria);

	return EXIT_SUCCESS;
}

