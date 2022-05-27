#ifndef PARSER_H_
#define PARSER_H_

#include <estructuras.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

t_list *parsear_pseudocodigo(char *path);
t_instruccion *generar_instruccion(char *identificador, uint32_t primer_operando, uint32_t segundo_operando);
t_identificador transformar_identificador(char *identificador);
void eliminar_instrucciones(t_list *instrucciones);


#endif /* PARSER_H_ */
