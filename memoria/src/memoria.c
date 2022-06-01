#include "memoria.h"


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
	t_pcb *pcb;
	t_paquete *paquete = datos_cliente->paquete;
	switch (paquete->codigo_operacion) {
		case INICIALIZACION_DE_PROCESO:
			pcb = deserializar_pcb(paquete);
			log_info(memoria_logger, "Inicializando estructuras de PID[%d]...", pcb->id);
			// TODO: crear las tablas de paginas, archivo swap y devolver la tabla de primer nivel
			enviar_numero_tabla_de_pagina(datos_cliente->socket, 20);
			eliminar_pcb(pcb);
			break;
		case SUSPENSION_DE_PROCESO:
			pcb = deserializar_pcb(paquete);
			log_info(memoria_logger, "Liberando memoria de PID[%d]...", pcb->id);
			// TODO: liberar memoria y escribir en SWAP lo datos necesarios
			informar_proceso_suspendido(datos_cliente->socket, PROCESO_SUSPENDIDO);

			eliminar_pcb(pcb);
			break;
		case FINALIZACION_DE_PROCESO:
			pcb = deserializar_pcb(paquete);
			log_info(memoria_logger, "Eliminando memoria de PID[%d]...", pcb->id);
			// TODO: liberar memoria y eliminar el archivo SWAP del proceso
			informar_proceso_suspendido(datos_cliente->socket, PROCESO_FINALIZADO);

			eliminar_pcb(pcb);
			break;
		case HANDSHAKE_INICIAL:
			log_info(memoria_logger, "Memoria recibio handshake... enviando estructura traductora");
			t_traductor *traductor = crear_traductor(memoria_config->entradas_por_tabla, memoria_config->tamanio_pagina);
			enviar_estructura_traductora(datos_cliente->socket, traductor);
			eliminar_traductor(traductor);
			break;
		case ACCESO_TABLA_PRIMER_NIVEL:
			log_info(memoria_logger, "Enviando tabla segundo nivel");
			// TODO: buscar tabla segundo nivel en tabla de nivel 1
			//t_tabla_pagina *tabla_pagina = deserializar_tabla_pagina(paquete);
			enviar_numero_tabla_de_pagina(datos_cliente->socket, 4);
			break;
		case ACCESO_TABLA_SEGUNDO_NIVEL:
			log_info(memoria_logger, "Enviando numero de marco");
			// TODO: buscar el marco en tabla segundo nivel
			//t_tabla_pagina *tabla_pagina = deserializar_tabla_pagina(paquete);
			enviar_numero_tabla_de_pagina(datos_cliente->socket, 5);
			break;
		case LEER_MEMORIA:
			log_info(memoria_logger, "Enviando valor de memoria");
			// TODO: devolver valor de memoria
			//t_tabla_pagina *tabla_pagina = deserializar_tabla_pagina(paquete);
			enviar_numero_tabla_de_pagina(datos_cliente->socket, 13);
			break;
		case ESCRIBIR_MEMORIA:
			log_info(memoria_logger, "Escribiendo en memoria");
			// TODO: escribir valor en memoria
			t_list *datos = deserealizar_paquete(paquete);
			log_info(memoria_logger, "Direccion fisica = %d", *(uint32_t *)list_get(datos, 0));
			log_info(memoria_logger, "Valor = %d", *(uint32_t *)list_get(datos, 1));
			list_destroy_and_destroy_elements(datos, free);
			break;
		default:
			log_error(memoria_logger, "Protocolo invalido.");
			break;
	}
	eliminar_paquete(paquete);
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
	t_traductor *traductor = malloc(sizeof(t_traductor));
	traductor->cantidad_entradas_tabla = entradas_tabla;
	traductor->tamanio_pagina = tamanio_pagina;

	return traductor;
}

void eliminar_traductor(t_traductor *traductor) {
	free(traductor);
}

void eliminar_pcb(t_pcb *pcb) {
	list_destroy_and_destroy_elements(pcb->instrucciones, free);
	free(pcb);
}

void informar_proceso_suspendido(int socket_fd, t_protocolo protocolo) {
	enviar_datos(socket_fd, &protocolo, sizeof(t_protocolo));
}


