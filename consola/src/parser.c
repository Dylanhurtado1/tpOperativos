#include "parser.h"

static t_identificador transformar_identificador(char *identificador);


t_list *parsear_pseudocodigo(char *path) {//recibe el path del archivo
	char identificador[10];
	uint32_t primer_operando;
	uint32_t segundo_operando;
	t_list *instrucciones = list_create();//lista de instrucciones es una lista

	FILE* archivo = fopen(path, "r");//se crea el archivo en modo lectura para leer el pseudocodigo
	if (archivo == NULL) {
		return NULL;
	}

	while (feof(archivo) == 0) {
		primer_operando = 0xFFFF;//fin de archivo1
		segundo_operando = 0xFFFF;//fin de archivo2
		fscanf(archivo, "%s %u %u", identificador, &primer_operando, &segundo_operando);
		if(string_equals_ignore_case(identificador, "NO_OP")) {//retorna si 2 strings son iguales
			for(int i = 0; i < primer_operando; i++) {
				t_instruccion *instruccion = crear_instruccion(transformar_identificador(identificador), 0xFFFF, 0xFFFF);
				list_add(instrucciones, instruccion);
			}
		} else {
			t_instruccion *instruccion = crear_instruccion(transformar_identificador(identificador), primer_operando, segundo_operando);
			list_add(instrucciones, instruccion);
		}
	}

	fclose(archivo);//se cierra el archivo

	return instrucciones;//retorna una lista con todas las instrucciones a ejecutar
}

t_identificador transformar_identificador(char *identificador) {//cada parametro lo retorna como un identificador, es un enum
	if(string_equals_ignore_case(identificador, "NO_OP")) {
		return NO_OP;
	}
	if(string_equals_ignore_case(identificador, "I/O") || string_equals_ignore_case(identificador, "IO")) {
		return IO;
	}
	if(string_equals_ignore_case(identificador, "READ")) {
		return READ;
	}
	if(string_equals_ignore_case(identificador, "WRITE")) {
		return WRITE;
	}
	if(string_equals_ignore_case(identificador, "COPY")) {
		return COPY;
	}
	if(string_equals_ignore_case(identificador, "EXIT")) {
		return EXIT;
	}

	return INVALID_ID;
}


