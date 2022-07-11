#include "cpu.h"

pthread_t th_dispatch;
pthread_t th_interrupt;


void sigint_handler(int sig_num) {
    log_info(cpu_logger, "Finalizando modulo CPU...");

	pthread_cancel(th_interrupt);
	pthread_cancel(th_dispatch);
}

void init(char *config_path) {
	pthread_mutex_init(&mutex_interrupt, NULL);
	interrupcion_desalojo = false;
	tlb = list_create();
	//cpu_logger = log_create("cpu.log", "CPU", true, LOG_LEVEL_INFO);
	cpu_config = cpu_leer_configuracion(config_path);
	ultimo_en_ejecutar = 0xFF;
	signal(SIGINT, sigint_handler);
}

int main(int argc, char **argv) {
	cpu_logger = log_create("cpu.log", "CPU", true, LOG_LEVEL_INFO);
	if(argc < 2) {
		log_error(cpu_logger, "Error de parametros. Ejemplo de uso: ./cpu <archivo_configuracion>");
		log_destroy(cpu_logger);
		return EXIT_FAILURE;
	}

	init(argv[1]);

	socket_memoria = conectar_a_modulo(cpu_config->ip_memoria, cpu_config->puerto_memoria, cpu_logger);
	traductor = obtener_traductor_direcciones(socket_memoria);

	int socket_dispatch = iniciar_modulo_servidor(cpu_config->ip_cpu, cpu_config->puerto_escucha_dispatch, cpu_logger);
	pthread_create(&th_dispatch, NULL, (void *)peticiones_dispatch, &socket_dispatch);

	int socket_interrupt = iniciar_modulo_servidor(cpu_config->ip_cpu, cpu_config->puerto_escucha_interrupt, cpu_logger);
	pthread_create(&th_interrupt, NULL, (void *)peticiones_interrupt, &socket_interrupt);

	pthread_join(th_dispatch, NULL);
	pthread_join(th_interrupt, NULL);

    tlb_limpiar_cache();
    list_destroy(tlb);
    log_destroy(cpu_logger);
	cpu_eliminar_configuracion(cpu_config);
	eliminar_traductor_direcciones(traductor);
	cerrar_conexion(socket_memoria);
	cerrar_conexion(socket_dispatch);
	cerrar_conexion(socket_interrupt);

	return EXIT_SUCCESS;
}

t_traductor *obtener_traductor_direcciones(int socket_fd) {
	realizar_handshake(socket_fd);
	t_paquete *paquete = recibir_paquete(socket_fd);
	t_traductor *traductor = deserializar_traductor(paquete);

	eliminar_paquete(paquete);

	return traductor;
}

void realizar_handshake(int socket_fd) {
	uint32_t fake_data = 0;
	t_paquete *paquete = crear_paquete(HANDSHAKE_INICIAL, buffer_vacio());
	agregar_a_paquete(paquete, &fake_data, sizeof(uint32_t)); // TODO: problemas con paquete sin datos
	enviar_paquete(paquete, socket_fd);
	eliminar_paquete(paquete);
}

