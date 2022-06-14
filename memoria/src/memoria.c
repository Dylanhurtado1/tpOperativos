#include "memoria.h"


int main(void) {

	init();

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

void init() {
	memoria_logger = log_create("memoria.log", "MEMORIA", true, LOG_LEVEL_INFO);
	memoria_config = memoria_leer_configuracion(PATH_MEMORIA_CONFIG);
	//memoria_principal = malloc(memoria_config->tamanio_memoria);
	//memset(memoria_principal, 0, memoria_config->tamanio_memoria);
	tablas_de_paginacion = list_create();
	iniciar_marcos_memoria();
}
