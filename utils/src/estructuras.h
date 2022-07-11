#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include <stdint.h>
#include <stdlib.h>
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

typedef enum {
	JOB,
	NEW,
	READY,
	EXEC,
	BLOCKED,
	_EXIT,
	SUSPENDED_BLOCKED,
	SUSPENDED_READY
} t_estado;

typedef struct {
	t_identificador identificador;
	uint32_t primer_operando;
	uint32_t segundo_operando;
} t_instruccion;//cada instruccion tiene esos atributos

typedef struct {
	t_list *instrucciones;
	uint32_t tamanio;
} t_consola;//la consonla tiene instrucciones y tamanio

typedef struct {
	uint32_t id;
	uint32_t tamanio_proceso;
	t_list *instrucciones;
	uint32_t program_counter;
	uint32_t tabla_paginas;
	uint32_t estimacion_rafaga;
} t_pcb;//un t_proceso tiene un t_pcb

typedef struct {
	uint32_t cantidad_entradas_tabla;
	uint32_t tamanio_pagina;
} t_traductor;//un traductor tiene esos atributos

typedef struct {
	uint32_t direccion;
	uint32_t entrada;
} t_tabla_acceso;//tiene el numero de tabla de primer nivel, y su entrada

t_instruccion *crear_instruccion(t_identificador identificador, uint32_t primer_operando, uint32_t segundo_operando);
void eliminar_instrucciones(t_list *instrucciones);
t_consola *crear_consola(t_list *instrucciones, uint32_t tamanio);
void eliminar_consola(t_consola *consola);
t_pcb *crear_pcb(uint32_t id, uint32_t tamanio_proceso, t_list *instrucciones, uint32_t program_counter, uint32_t estimacion_rafaga);
void eliminar_pcb(t_pcb *pcb);
t_traductor *crear_traductor_direcciones(int entradas_tabla, int tamanio_pagina);
void eliminar_traductor_direcciones(t_traductor *traductor);
t_tabla_acceso *crear_tabla_de_acceso(uint32_t direccion, uint32_t entrada);
void eliminar_tabla_de_acceso(t_tabla_acceso *tabla_acceso);

#endif /* ESTRUCTURAS_H_ */

