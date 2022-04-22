#include "cpu.h"
#include <unistd.h>

t_log *cpu_logger;

int main(void) {
	cpu_logger = log_create("cpu.log", "CPU", true, LOG_LEVEL_INFO);
	t_cpu_config *config = cpu_leer_configuracion(PATH_CPU_CONFIG);

	int socket_memoria = conectar_a_memoria(config->ip_memoria, config->puerto_memoria);
	if(socket_memoria == SERVER_CONNECTION_ERROR) {
		log_error(cpu_logger, "Error al conectar con Memoria");
		log_destroy(cpu_logger);
		return EXIT_FAILURE;
	}
	t_traductor *traductor = obtener_traductor_direcciones(socket_memoria);

	int socket_dispatch = iniciar_servidor_dispatch(config->ip_cpu, config->puerto_escucha_dispatch);
	if(socket_dispatch == INIT_SERVER_ERROR) {
		log_error(cpu_logger, "No se pudo iniciar el servidor de Dispatch");
		return EXIT_FAILURE;
	}
	log_info(cpu_logger, "CPU escuchando puerto dispatch");

	int socket_interrupt = iniciar_servidor_interrupt(config->ip_cpu, config->puerto_escucha_interrupt);
	if(socket_interrupt == INIT_SERVER_ERROR) {
		log_error(cpu_logger, "No se pudo iniciar el servidor de Interrupt");
		return EXIT_FAILURE;
	}
	log_info(cpu_logger, "CPU escuchando puerto interrupt");

	while(1);

	log_destroy(cpu_logger);
	cpu_eliminar_configuracion(config);
	eliminar_traductor_direcciones(traductor);
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

t_traductor *obtener_traductor_direcciones(int socket_fd) {
	realizar_handshake(socket_fd);
	t_paquete *paquete = recibir_paquete(socket_fd);
	t_traductor *traductor = deserealizar_traductor(paquete);

	eliminar_paquete(paquete);

	log_info(cpu_logger, "Datos de traductor: cantidas entradas = %d, tamanio pagina = %d",
			traductor->cantidad_entradas_tabla, traductor->tamanio_pagina);

	return traductor;
}

void realizar_handshake(int socket_fd) {
	uint32_t fake_data = 0;
	t_paquete *paquete = crear_paquete(HANDSHAKE_INICIAL, buffer_vacio());
	agregar_a_paquete(paquete, &fake_data, sizeof(uint32_t)); // TODO: problemas con paquete sin datos
	enviar_paquete(paquete, socket_fd);
	eliminar_paquete(paquete);
}

t_traductor *deserealizar_traductor(t_paquete *paquete) {
	t_list *datos = deserealizar_paquete(paquete);
	t_traductor *traductor = malloc(sizeof(t_traductor));
	traductor->cantidad_entradas_tabla = *(uint32_t *)list_get(datos, 0);
	traductor->tamanio_pagina = *(uint32_t *)list_get(datos, 1);

	list_destroy_and_destroy_elements(datos, free);

	return traductor;
}

void eliminar_traductor_direcciones(t_traductor *traductor) {
	free(traductor);
}



