#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <estructuras.h>

t_list *parsear_pseudocodigo(char *path);
void eliminar_instrucciones(t_list *instrucciones);
t_instruccion *generar_instruccion(char *identificador, uint32_t primer_operando, uint32_t segundo_operando);
t_identificador transformar_identificador(char *identificador);

#endif /* PARSER_H_ */
