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

int conectar_a_kernel(char *ip, char *puerto);
void enviar_datos_a_kernel(int socket_kernel, t_list *instrucciones, uint32_t tam_consola);
t_protocolo esperar_respuesta_de_kernel(int socket_kernel);
t_paquete *serializar_datos_consola(t_list *instrucciones, uint32_t tam_consola);

int main(int argc, char **argv) {
	t_log *logger = log_create("consola.log", "CONSOLA", true, LOG_LEVEL_INFO);
	t_consola_config *config = consola_leer_configuracion(PATH_CONSOLA_CONFIG);

	if(argc < 2) {
		log_error(logger, "Error de parametros. Ejemplo de uso: ./consola <ruta_a_pseudocodigo> <tam_proceso>");
		log_destroy(logger);
		return EXIT_FAILURE;
	}

	t_list *instrucciones = parsear_instrucciones(argv[1]);
	uint32_t tam_consola = atoi(argv[2]);


	int socket_kernel = conectar_a_kernel(config->ip_kernel, config->puerto_kernel);
	if(socket_kernel == SERVER_CONNECTION_ERROR) {
		log_error(logger, "Error al conectar con Kernel");
		log_destroy(logger);
		return EXIT_FAILURE;
	}

	enviar_datos_a_kernel(socket_kernel, instrucciones, tam_consola);
	t_protocolo respuesta = esperar_respuesta_de_kernel(socket_kernel);
	if(respuesta != FINALIZAR_CONSOLA_OK) {
		log_error(logger, "Error al finalizar consola");
	}

	log_destroy(logger);
	consola_eliminar_configuracion(config);
	eliminar_instrucciones(instrucciones);
	cerrar_conexion(socket_kernel);

	return EXIT_SUCCESS;

}

int conectar_a_kernel(char *ip, char *puerto) {
	return conectar_a_servidor(ip, puerto);
}

t_protocolo esperar_respuesta_de_kernel(int socket_kernel) {
	return recibir_operacion(socket_kernel);
}

void enviar_datos_a_kernel(int socket_kernel, t_list *instrucciones, uint32_t tam_consola) {
	t_paquete *paquete = serializar_datos_consola(instrucciones, tam_consola);

	enviar_paquete(paquete, socket_kernel);

	eliminar_paquete(paquete);
}

t_paquete *serializar_datos_consola(t_list *instrucciones, uint32_t tam_consola) {
	t_paquete *paquete = crear_paquete(DATOS_CONSOLA, buffer_vacio());

	for(int i = 0; i < list_size(instrucciones); i++) {
		t_instruccion *instruccion = (t_instruccion *)list_get(instrucciones, i);
		agregar_a_paquete(paquete, &(instruccion->identificador), sizeof(t_identificador));
		agregar_a_paquete(paquete, &(instruccion->primer_operando), sizeof(t_identificador));
		agregar_a_paquete(paquete, &(instruccion->segundo_operando), sizeof(t_identificador));
	}
	agregar_a_paquete(paquete, &tam_consola, sizeof(tam_consola));

	return paquete;
}


