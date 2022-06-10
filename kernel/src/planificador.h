#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <unistd.h>
#include <servidor.h>
#include <protocolo.h>
#include <comunicacion.h>
#include <estructuras.h>
#include <serializador.h>
#include "kernel_global.h"


typedef struct {
	int socket;
	t_pcb *pcb;
	t_estado estado;
	uint32_t tiempo_io;
	uint32_t tiempo_inicio_bloqueo;
	uint32_t tiempo_cpu;
} t_proceso;


// Planificador Largo Plazo
void iniciar_planificador_largo_plazo();
t_proceso *crear_proceso(t_consola *consola, int socket_consola);
t_pcb *crear_estructura_pcb(t_consola *consola);
void agregar_proceso_a_new(t_proceso *proceso);
void transicion_admitir(void *data);
uint32_t obtener_tabla_de_pagina(int socket_memoria, t_pcb *pcb);
void estado_exit(void *dato);
void enviar_respuesta_a_consola(int socket_fd, t_protocolo protocolo);
void enviar_proceso_a_memoria(t_proceso *proceso, int socket_memoria, t_protocolo protocolo);
t_protocolo esperar_respuesta_memoria(int socket_memoria);
void eliminar_proceso(t_proceso *proceso);
void eliminar_pcb(t_pcb *pcb);


// Planificador Corto Plazo
void iniciar_planificador_corto_plazo();
void estado_ready(void *data);
void estado_exec(void *data);
void estado_blocked(void *data);
void enviar_proceso_a_cpu(t_proceso *proceso, int socket_cpu_dispatch);
t_paquete *esperar_respuesta_cpu(int socket_cpu_dispatch);
void enviar_interrupcion_a_cpu(int socket_fd);
t_proceso *siguiente_a_ejecutar(char *algoritmo);


// Planificador Mediano Plazo
void iniciar_planificador_mediano_plazo();
void transicion_suspender(t_proceso *proceso);
void estado_suspended_ready(void *data);


#endif /* PLANIFICADOR_H_ */
