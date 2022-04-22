#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <estructuras.h>

t_list *parsear_pseudocodigo(char *path);
void eliminar_instrucciones(t_list *instrucciones);

#endif /* PARSER_H_ */
