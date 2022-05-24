#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <unistd.h>
#include <servidor.h>
#include <protocolo.h>
#include <comunicacion.h>
#include <estructuras.h>
#include <serializador.h>
#include "kernel_global.h"

// Planificador Largo Plazo
void iniciar_planificador_largo_plazo();
t_pcb *crear_estructura_pcb(t_list *instrucciones, uint32_t tam_proceso);
void agregar_proceso_a_new(t_pcb *proceso, int socket_fd);
void transicion_admitir(void *data);
uint32_t obtener_entrada_tabla_de_pagina(int socket_fd);
void estado_exit(void *dato);
void enviar_respuesta_a_consola(int socket_fd, t_protocolo protocolo);
void eliminar_proceso(t_pcb *proceso);
void enviar_proceso_a_memoria(t_pcb *pcb, int socket_memoria, t_protocolo protocolo);
t_protocolo esperar_respuesta_memoria(int socket_memoria);


// Planificador Corto Plazo
void iniciar_planificador_corto_plazo();
void estado_ready(void *data);
void estado_exec(void *data);
void estado_blocked(void *data);
void enviar_proceso_a_cpu(t_pcb *pcb, int socket_cpu_dispatch);
t_paquete *esperar_respuesta_cpu(int socket_cpu_dispatch);
void enviar_interrupcion_a_cpu(int socket_fd);


// Planificador Mediano Plazo
void iniciar_planificador_mediano_plazo();
void transicion_suspender(t_pcb *pcb);
void estado_suspended_ready(void *data);

typedef struct {
	int socket;
	uint32_t id;
} t_pid;

#endif /* PLANIFICADOR_H_ */
