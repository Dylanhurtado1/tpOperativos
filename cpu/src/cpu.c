/*
 ============================================================================
 Name        : cpu.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "cpu.h"

#define PATH_CPU_CONFIG "/home/utnso/tp-2022-1c-lo-importante-es-aprobar/cpu/cpu.config"

int conectar_a_memoria(char *ip, char *puerto);
int iniciar_servidor_dispatch(char *ip, char *puerto);
int iniciar_servidor_interrupt(char *ip, char *puerto);

int main(void) {
	t_log *logger = log_create("cpu.log", "CPU", true, LOG_LEVEL_INFO);
	t_cpu_config *config = cpu_leer_configuracion(PATH_CPU_CONFIG);

	int socket_memoria = conectar_a_memoria(config->ip_memoria, config->puerto_memoria);
	if(socket_memoria == SERVER_CONNECTION_ERROR) {
		log_error(logger, "Error al conectar con Memoria");
		log_destroy(logger);
		return EXIT_FAILURE;
	}
	// TODO: realizar handshake inicial

	int socket_dispatch = iniciar_servidor_dispatch(config->ip_cpu, config->puerto_escucha_dispatch);
	if(socket_dispatch == INIT_SERVER_ERROR) {
		log_error(logger, "No se pudo iniciar el servidor de Dispatch");
		return EXIT_FAILURE;
	}

	int socket_interrupt = iniciar_servidor_interrupt(config->ip_cpu, config->puerto_escucha_interrupt);
	if(socket_interrupt == INIT_SERVER_ERROR) {
		log_error(logger, "No se pudo iniciar el servidor de Interrupt");
		return EXIT_FAILURE;
	}

	log_destroy(logger);
	cpu_eliminar_configuracion(config);
	cerrar_conexion(socket_memoria);
	cerrar_conexion(socket_dispatch);
	cerrar_conexion(socket_interrupt);

	return EXIT_SUCCESS;
}

int conectar_a_memoria(char *ip, char *puerto) {
	return conectar_a_servidor(ip, puerto);
}

int iniciar_servidor_dispatch(char *ip, char *puerto) {
	return crear_servidor(ip, puerto);
}

int iniciar_servidor_interrupt(char *ip, char *puerto) {
	return crear_servidor(ip, puerto);
}



