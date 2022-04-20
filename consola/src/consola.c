/*
 ============================================================================
 Name        : consola.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "consola.h"

#define PATH_CONSOLA_CONFIG "/home/utnso/tp-2022-1c-lo-importante-es-aprobar/consola/consola.config"

void enviar_datos_a_kernel(int socket_kernel, t_paquete *paquete);
t_protocolo esperar_respuesta_de_kernel(int socket_kernel);
int conectar_a_kernel(char *ip, char *puerto);

int main(int argc, char **argv) {
	t_log *logger = log_create("consola.log", "CONSOLA", true, LOG_LEVEL_INFO);
	t_paquete *paquete;

	if(argc < 2) {
		log_error(logger, "Error de parametros. Ejemplo de uso: ./consola <ruta_a_pseudocodigo> <tam_proceso>");
		log_destroy(logger);
		return EXIT_FAILURE;
	}

	parsear_instrucciones(argv[1]);

	t_consola_config *config = consola_leer_configuracion(PATH_CONSOLA_CONFIG);

	int socket_kernel = conectar_a_kernel(config->ip_kernel, config->puerto_kernel);
	if(socket_kernel == SERVER_CONNECTION_ERROR) {
		log_error(logger, "Error al conectar con Kernel");
		log_destroy(logger);
		return EXIT_FAILURE;
	}

	enviar_datos_a_kernel(socket_kernel, paquete);
	t_protocolo respuesta = esperar_respuesta_de_kernel(socket_kernel);
	if(respuesta != FINALIZAR_CONSOLA_OK) {
		log_error(logger, "Error al finalizar consola");
	}

	log_destroy(logger);
	consola_eliminar_configuracion(config);
	cerrar_conexion(socket_kernel);

	return EXIT_SUCCESS;

}

int conectar_a_kernel(char *ip, char *puerto) {
	return conectar_a_servidor(ip, puerto);
}

void enviar_datos_a_kernel(int socket_kernel, t_paquete *paquete) {
	enviar_mensaje("hola cesar 2", DATOS_CONSOLA, socket_kernel);
}

t_protocolo esperar_respuesta_de_kernel(int socket_kernel) {
	return recibir_operacion(socket_kernel);
}

