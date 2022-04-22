#include "parser.h"

t_identificador transformar_identificador(char *identificador);
void imprimir(void *dato);

t_list *parsear_pseudocodigo(char *path) {
	char identificador[10];
	uint32_t primer_operando;
	uint32_t segundo_operando;
	t_list *lista_instrucciones = list_create();

	FILE* archivo = fopen(path, "r");
	if (archivo == NULL) {
		return NULL;
	}
	while (feof(archivo) == 0) {
		primer_operando = 0xFFFF;
		segundo_operando = 0xFFFF;
		fscanf(archivo, "%s %u %u", identificador, &primer_operando, &segundo_operando);
		t_instruccion *instruccion = malloc(sizeof(t_instruccion));
		instruccion->identificador = transformar_identificador(identificador);
		instruccion->primer_operando = primer_operando;
		instruccion->segundo_operando = segundo_operando;
		list_add(lista_instrucciones, instruccion);
	}

	list_iterate(lista_instrucciones, imprimir);

	fclose(archivo);

	return lista_instrucciones;
}

void imprimir(void *dato) {
	t_instruccion *instruccion = (t_instruccion *)dato;
	printf("\nIdentificador: %d, op_1: %d, op_2: %d\n", instruccion->identificador, instruccion->primer_operando, instruccion->segundo_operando);
}

t_identificador transformar_identificador(char *identificador) {
	if(string_equals_ignore_case(identificador, "NO_OP")) {
		return NO_OP;
	}
	if(string_equals_ignore_case(identificador, "I/O")) {
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

void eliminar_instrucciones(t_list *instrucciones) {
	list_destroy_and_destroy_elements(instrucciones, free);
}


