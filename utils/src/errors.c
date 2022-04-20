/*
 * errors.c
 *
 *  Created on: 23 mar. 2022
 *      Author: utnso
 */

#include "errors.h"

bool es_codigo_de_error(t_socket_error codigo) {
	switch(codigo) {
		case SOCKET_CREATION_ERROR:
		case LISTEN_ERROR:
		case CONNECT_ERROR:
		case ACCEPT_ERROR:
		case RECEIVE_DATA_ERROR:
		case SEND_DATA_ERROR:
		case INIT_SERVER_ERROR:
		case WAIT_CLIENT_ERROR:
		case SERVER_CONNECTION_ERROR:
			return true;
	}
	return false;
}
