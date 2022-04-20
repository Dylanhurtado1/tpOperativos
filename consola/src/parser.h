/*
 * parser.h
 *
 *  Created on: 19 abr. 2022
 *      Author: utnso
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <commons/string.h>
#include <commons/collections/list.h>

typedef enum {
	NO_OP,
	IO,
	READ,
	WRITE,
	COPY,
	EXIT,
	INVALID_ID
} t_identificador;

typedef struct {
	t_identificador identificador;
	uint32_t primer_operando;
	uint32_t segundo_operando;
} t_instruccion;

t_list *parsear_instrucciones(char *path);
void eliminar_instrucciones(t_list *instrucciones);

#endif /* PARSER_H_ */
