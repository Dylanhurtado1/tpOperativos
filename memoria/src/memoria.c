#include "memoria.h"

void init();


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
}

void procesar_conexiones(t_cliente *datos_cliente) {
	t_pcb *pcb;
	t_acceso_tabla *tabla;
	t_list *datos;

	t_paquete *paquete = datos_cliente->paquete;
	switch (paquete->codigo_operacion) {
		case INICIALIZACION_DE_PROCESO:
			pcb = deserializar_pcb(paquete);
			log_info(memoria_logger, "Inicializando estructuras de PID[%d]...", pcb->id);
			uint32_t tabla_primer_nivel = crear_tablas_de_paginacion(pcb);
			// TODO: crear archivo swap
			enviar_tabla_de_pagina(datos_cliente->socket, tabla_primer_nivel);

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
			log_info(memoria_logger, "Memoria recibio handshake, enviando estructura traductora...");
			t_traductor *traductor = crear_traductor(memoria_config->entradas_por_tabla, memoria_config->tamanio_pagina);
			enviar_estructura_traductora(datos_cliente->socket, traductor);

			eliminar_traductor(traductor);
			break;
		case ACCESO_TABLA_PRIMER_NIVEL:
			log_info(memoria_logger, "Enviando tabla segundo nivel...");
			tabla = deserializar_acceso_tabla(paquete);
			uint32_t tabla_segundo_nivel = get_tabla_segundo_nivel(tabla->direccion, tabla->entrada);
			enviar_tabla_de_pagina(datos_cliente->socket, tabla_segundo_nivel);

			eliminar_acceso_tabla(tabla);
			break;
		case ACCESO_TABLA_SEGUNDO_NIVEL:
			log_info(memoria_logger, "Enviando numero de marco...");
			tabla = deserializar_acceso_tabla(paquete);
			uint32_t marco = get_marco_de_pagina(tabla->direccion, tabla->entrada);
			enviar_marco_de_pagina(datos_cliente->socket, marco);

			eliminar_acceso_tabla(tabla);
			break;
		case LEER_MEMORIA:
			log_info(memoria_logger, "Enviando valor de memoria...");
			datos = deserealizar_paquete(paquete);
			log_info(memoria_logger, "Direccion fisica a leer = %d", *(uint32_t *)list_get(datos, 0));
			// TODO: devolver valor de memoria
			enviar_valor_de_memoria(datos_cliente->socket, 13);

			list_destroy_and_destroy_elements(datos, free);
			break;
		case ESCRIBIR_MEMORIA:
			log_info(memoria_logger, "Escribiendo valor en memoria...");
			datos = deserealizar_paquete(paquete);
			log_info(memoria_logger, "Direccion fisica a escribir = %d", *(uint32_t *)list_get(datos, 0));
			log_info(memoria_logger, "Valor = %d", *(uint32_t *)list_get(datos, 1));
			// TODO: escribir valor en memoria

			list_destroy_and_destroy_elements(datos, free);
			break;
		default:
			log_error(memoria_logger, "Protocolo invalido.");
			break;
	}
	eliminar_paquete(paquete);
}

void enviar_tabla_de_pagina(int socket_fd, uint32_t numero) {
	enviar_datos(socket_fd, &numero, sizeof(uint32_t));
}

void enviar_marco_de_pagina(int socket_fd, uint32_t numero) {
	enviar_datos(socket_fd, &numero, sizeof(uint32_t));
}

void enviar_valor_de_memoria(int socket_fd, uint32_t valor) {
	enviar_datos(socket_fd, &valor, sizeof(uint32_t));
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

void eliminar_acceso_tabla(t_acceso_tabla *acceso_tabla) {
	free(acceso_tabla);
}
