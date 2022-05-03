#include "memoria.h"

t_log *memoria_logger;
t_memoria_config *memoria_config;

int main(void) {
	memoria_logger = log_create("memoria.log", "MEMORIA", true, LOG_LEVEL_INFO);
	memoria_config = memoria_leer_configuracion(PATH_MEMORIA_CONFIG);

	int socket_memoria = iniciar_modulo_servidor(memoria_config->ip_memoria, memoria_config->puerto_escucha, memoria_logger);
	log_info(memoria_logger, "Memoria iniciado como servidor");

	if(atender_clientes(socket_memoria, procesar_conexiones) == WAIT_CLIENT_ERROR) {
		log_error(memoria_logger, "Error al escuchar clientes... Finalizando servidor");
	}

	log_destroy(memoria_logger);
	memoria_eliminar_configuracion(memoria_config);
	cerrar_conexion(socket_memoria);

	return EXIT_SUCCESS;
}

void procesar_conexiones(t_cliente *datos_cliente) {
	t_paquete *paquete = datos_cliente->paquete;
	switch (paquete->codigo_operacion) {
		case AGREGAR_PROCESO_A_MEMORIA:
			log_info(memoria_logger, "Creando estructuras de Proceso");
			// TODO: crear la tabla de paginas para devolver el numero a Kernel
			enviar_numero_tabla_de_pagina(datos_cliente->socket, 2);
			break;
		case HANDSHAKE_INICIAL:
			log_info(memoria_logger, "Memoria recibio handshake... enviando estructura traductora");
			t_traductor *traductor = crear_traductor(memoria_config->paginas_por_tabla, memoria_config->tam_pagina);
			enviar_estructura_traductora(datos_cliente->socket, traductor);
			eliminar_traductor(traductor);
			break;
		default:
			log_error(memoria_logger,"Protocolo invalido.");
			break;
	}
}

void enviar_numero_tabla_de_pagina(int socket_fd, uint32_t numero) {
	enviar_datos(socket_fd, &numero, sizeof(uint32_t));
}

void enviar_estructura_traductora(int socket_fd, t_traductor *traductor) {
	t_paquete *paquete = serializar_traductor(traductor, HANDSHAKE_INICIAL);
	enviar_paquete(paquete, socket_fd);
	eliminar_paquete(paquete);
}

t_traductor *crear_traductor(int entradas_tabla, int tamanio_pagina) {
	t_traductor *traductor = (t_traductor *)malloc(sizeof(t_traductor));
	traductor->cantidad_entradas_tabla = entradas_tabla;
	traductor->tamanio_pagina = tamanio_pagina;

	return traductor;
}

void eliminar_traductor(t_traductor *traductor) {
	free(traductor);
}



