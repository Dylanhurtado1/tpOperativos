/*
 * parser.c
 *
 *  Created on: 19 abr. 2022
 *      Author: utnso
 */

#include "parser.h"

void parser(char *path) {
	Instruccion instruccion;
	char instruccionLeida[10];
	unsigned long int primerArgumento;
	unsigned long int segundoArgumento;
	//t_list listInstrucciones = *list_create();


	//int tamProceso = argv[2]; //Es el tamaño del proceso pasado por parametro, chequear que sea int u otro tipo (probablemente)
	FILE* archivo = fopen(path, "r");
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

		printf("\nSe encontro la instruccion %s con el parametro %lu y %lu\n", //Hay que guardarlas en una lista, no imprimirlas (es para ver si parsea bien)
				instruccionLeida, primerArgumento, segundoArgumento);

	}
	fclose(archivo);


	printf("\nTermine de leer el archivo de instrucciones\n");
}
