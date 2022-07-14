#include "consola.h"
#include <print.h>


int main(int argc, char **argv) {
	t_log *consola_logger = log_create("consola.log", "CONSOLA", true, LOG_LEVEL_INFO);
	t_consola_config *config = consola_leer_configuracion(PATH_CONSOLA_CONFIG);

	if(argc < 3) {
		log_error(consola_logger, "Error de parametros. Ejemplo de uso: ./consola <ruta_a_pseudocodigo> <tam_proceso>");
		log_destroy(consola_logger);
		consola_eliminar_configuracion(config);
		return EXIT_FAILURE;
	}

	t_consola *consola = crear_consola(parsear_pseudocodigo(argv[1]), atoi(argv[2]));
	print_instrucciones(consola_logger, consola->instrucciones);

	int socket_kernel = conectar_a_modulo(config->ip_kernel, config->puerto_kernel, consola_logger);
	enviar_consola_a_kernel(socket_kernel, consola);

	t_protocolo respuesta = esperar_respuesta_de_kernel(socket_kernel);
	if(respuesta != FINALIZAR_CONSOLA) {
		log_error(consola_logger, "Error al finalizar consola");
	}

	log_destroy(consola_logger);
	consola_eliminar_configuracion(config);
	eliminar_consola(consola);
	cerrar_conexion(socket_kernel);

	return EXIT_SUCCESS;
}

void enviar_consola_a_kernel(int socket_kernel, t_consola *consola) {
	t_paquete *paquete = serializar_consola(consola, DATOS_CONSOLA);

	enviar_paquete(paquete, socket_kernel);
	eliminar_paquete(paquete);
}

t_protocolo esperar_respuesta_de_kernel(int socket_kernel) {
	return recibir_operacion(socket_kernel);
}

