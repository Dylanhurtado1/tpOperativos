/*
 ============================================================================
 Name        : consola.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <cliente.h>
#include <commons/log.h>

int main(int argc, char **argv) {
	t_log *logger = log_create("server.log", "SERVER", true, LOG_LEVEL_INFO);
	conectar_a_servidor("", "");

	return EXIT_SUCCESS;
}
