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

typedef enum {
	NO_OP,
	IO,
	READ,
	COPY,
	WRITE,
	EXIT
} Instruccion;

void parsear_instrucciones(char *path);

#endif /* PARSER_H_ */
