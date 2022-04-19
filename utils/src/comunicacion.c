#include "comunicacion.h"
#include "errors.h"

void enviar_mensaje(char *mensaje, uint8_t codigo_operacion, int socket_conexion) {
	t_paquete *paquete = crear_paquete(codigo_operacion, buffer_vacio());

	agregar_a_paquete(paquete, mensaje, strlen(mensaje) + 1);
    enviar_paquete(paquete, socket_conexion);
	eliminar_paquete(paquete);
}

void enviar_paquete(t_paquete *paquete, int socket_conexion) {
	t_buffer *a_enviar = serializar_paquete(paquete);

	enviar_datos(socket_conexion, a_enviar->stream, a_enviar->size);
	eliminar_buffer(a_enviar);
}

t_paquete *recibir_paquete(int socket_cliente) {
	uint8_t cod_op = recibir_operacion(socket_cliente);
	if(cod_op == RECEIVE_DATA_ERROR)
		return crear_paquete(cod_op, buffer_vacio());
	t_buffer *buffer = recibir_buffer(socket_cliente);
	t_paquete *paquete = crear_paquete(cod_op, buffer);

	return paquete;
}

uint8_t recibir_operacion(int socket_cliente) {
	uint8_t codigo_operacion;

	if(recibir_datos(socket_cliente, &codigo_operacion, sizeof(uint8_t)) <= 0) {
		return RECEIVE_DATA_ERROR;
	}

	return codigo_operacion;
}

t_buffer *recibir_buffer(int socket_cliente) {
	t_buffer *buffer = buffer_vacio();

	recibir_datos(socket_cliente, &(buffer->size), sizeof(uint32_t));
	buffer->stream = malloc(buffer->size);
	recibir_datos(socket_cliente, buffer->stream, buffer->size);
	return buffer;
}


