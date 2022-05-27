#include "cpu.h"


int main(void) {
	cpu_logger = log_create("cpu.log", "CPU", true, LOG_LEVEL_INFO);
	cpu_config = cpu_leer_configuracion(PATH_CPU_CONFIG);
	pthread_t th_dispatch;
	pthread_t th_interrupt;
	sem_init(&sem_interrupt, 0, 1);
	interrupcion_desalojo = false;

	socket_memoria = conectar_a_modulo(cpu_config->ip_memoria, cpu_config->puerto_memoria, cpu_logger);

	traductor = obtener_traductor_direcciones(socket_memoria);

	int socket_dispatch = iniciar_modulo_servidor(cpu_config->ip_cpu, cpu_config->puerto_escucha_dispatch, cpu_logger);
	pthread_create(&th_dispatch, NULL, (void *)peticiones_dispatch, &socket_dispatch);

	int socket_interrupt = iniciar_modulo_servidor(cpu_config->ip_cpu, cpu_config->puerto_escucha_interrupt, cpu_logger);
	pthread_create(&th_interrupt, NULL, (void *)peticiones_interrupt, &socket_interrupt);

	pthread_join(th_dispatch, NULL);
	pthread_join(th_interrupt, NULL);

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

void eliminar_traductor_direcciones(t_traductor *traductor) {
	free(traductor);
}



