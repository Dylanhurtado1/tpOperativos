/*
 * servidor.c
 *
 *  Created on: 22 mar. 2022
 *      Author: utnso
 */

#include "servidor.h"

static t_socket *crear_socket_conexion(int socket_fd, void (*callback)(t_cliente *));
static t_cliente *crear_estructura_cliente(int socket_fd, t_paquete *paquete);
static void eliminar_socket_conexion(t_socket *conexion);
static void ejecutar_instruccion(t_socket *conexion);
static bool error_conexion(t_paquete *paquete);

int crear_servidor(char *ip, char *puerto) {
    int socket_servidor = iniciar_servidor(ip, puerto);

	//if(socket_servidor == SOCKET_CREATION_ERROR || socket_servidor == LISTEN_ERROR) {
    if(es_codigo_de_error(socket_servidor)) {
		return INIT_SERVER_ERROR;
	}

    return socket_servidor;
}

int atender_clientes(int socket_servidor, void (*callback)(t_cliente *)) {
    while(true) {
    	int socket_cliente = esperar_cliente(socket_servidor);
        //if(socket_cliente == ACCEPT_ERROR) {
        if(es_codigo_de_error(socket_cliente)) {
        	break;
        }
        pthread_t th_cliente;
        t_socket *conexion = crear_socket_conexion(socket_cliente, callback);

	    pthread_create(&th_cliente, NULL, (void *)ejecutar_instruccion, conexion);
	    pthread_detach(th_cliente);
    }

    return WAIT_CLIENT_ERROR;
}

static t_socket *crear_socket_conexion(int socket_fd, void (*callback)(t_cliente *)) {
    t_socket *conexion = malloc(sizeof(t_socket));
    conexion->socket = socket_fd;
    conexion->callback = callback;
    return conexion;
}

static void eliminar_socket_conexion(t_socket *conexion) {
	free(conexion);
}

static t_cliente *crear_estructura_cliente(int socket_fd, t_paquete *paquete) {
    t_cliente *cliente = malloc(sizeof(t_cliente));
    cliente->socket = socket_fd;
    cliente->paquete = paquete;
    return cliente;
}

static void eliminar_estructura_cliente(t_cliente *datos_cliente) {
	free(datos_cliente);
}

static void ejecutar_instruccion(t_socket *conexion) {
	t_paquete *paquete;

	while(true) {
		paquete = recibir_paquete(conexion->socket);
		if(error_conexion(paquete)) {
	    	break;
	    }
		t_cliente *datos_cliente = crear_estructura_cliente(conexion->socket, paquete);
	    conexion->callback(datos_cliente);
	    eliminar_paquete(paquete);
	    eliminar_estructura_cliente(datos_cliente);
	}

	eliminar_paquete(paquete);
	cerrar_conexion(conexion->socket);
	eliminar_socket_conexion(conexion);
}

static bool error_conexion(t_paquete *paquete) {
	return paquete == (t_paquete *)NULL || es_codigo_de_error(paquete->codigo_operacion);
}


