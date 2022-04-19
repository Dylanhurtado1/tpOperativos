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
#include <sockets.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>

typedef enum {
	NO_OP, IO,    //En tp dice I/O pero no me toma la / para que sea un enum
	READ,
	COPY,
	WRITE,
	EXIT
} Instruccion;

//Queria armar un switch con cada caso pero era un bardo, por las dudas lo deje pero no hace nada ahora

int main(int argc, char **argv) { //argc es la cantidad de argumentos, debe ser 2 segun el tp (1. el path al pseudocodigo y 2. el tamaño

	Instruccion instruccion;
	char instruccionLeida[10];
	unsigned long int primerArgumento;
	unsigned long int segundoArgumento;
	//t_list listInstrucciones = *list_create();

	if (argc < 2) {
		return EXIT_FAILURE;
	}

	else {
		int tamProceso = argv[2]; //Es el tamaño del proceso pasado por parametro, chequear que sea int u otro tipo (probablemente)
		FILE* archivo = fopen(argv[1], "r");
		//t_config* config = config_create(argv[1]); //Creo la config pero con el path
		if (archivo == NULL) {
			printf(
					"Ruta de archivo de pseudocodigo inexistente. Por favor revise la ruta");
			exit(2);
		}
		while (feof(archivo) == 0) {
			fscanf(archivo, "%s%lu%lu", &instruccionLeida, &primerArgumento,
					&segundoArgumento);
			if (primerArgumento == NULL && segundoArgumento == NULL) {
				printf("Se encontro la instruccion EXIT");

			}

			printf(
					"\nSe encontro la instruccion %s con el parametro %lu y %lu\n", //Hay que guardarlas en una lista, no imprimirlas (es para ver si parsea bien)
					instruccionLeida, primerArgumento, segundoArgumento);

		}
		fclose(archivo);

	}

	printf("\nTermine de leer el archivo de instrucciones\n");

	/*t_log *logger = log_create("consola.log", "SERVER", true, LOG_LEVEL_INFO);
	 int server_fd = conectar_a_servidor("", "");

	 log_info(logger, "Consola iniciado");

	 log_destroy(logger);
	 cerrar_conexion(server_fd);

	 return EXIT_SUCCESS;
	 */

}


