#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include <stdint.h>
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
} t_instruccion;

typedef struct {
	t_list *instrucciones;
	uint32_t tamanio;
} t_consola;

typedef struct {
	uint32_t id;
	uint32_t tamanio_proceso;
	t_list *instrucciones;
	uint32_t program_counter;
	uint32_t tabla_paginas;
	uint32_t estimacion_rafaga;
	uint32_t tiempo_io;
	uint32_t tiempo_inicio_bloqueo;
	t_estado estado;
} t_pcb;

typedef struct {
	uint32_t cantidad_entradas_tabla;
	uint32_t tamanio_pagina;
} t_traductor;

typedef struct {
	uint32_t direccion;
	uint32_t entrada;
} t_tabla_acceso;


#endif /* ESTRUCTURAS_H_ */

