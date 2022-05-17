#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <sockets.h>
#include <servidor.h>
#include <protocolo.h>
#include <comunicacion.h>
#include <estructuras.h>
#include <serializador.h>
#include "kernel_config.h"

#include "kernel_global.h"

// Planificador Largo Plazo
void iniciar_planificador_largo_plazo();
void agregar_proceso_a_new(t_pcb *proceso, int socket_fd);
t_pcb *crear_estructura_pcb(t_list *instrucciones, uint32_t tam_proceso);
void transicion_admitir(void *data);
bool es_posible_admitir_proceso();
void admitir_proceso();
void eliminar_proceso(t_pcb *proceso);
void estado_exit(void *dato);
void enviar_respuesta_a_consola(int socket_fd, t_protocolo protocolo);
uint32_t obtener_numero_tabla_de_pagina(int socket_fd);
//void iniciar_cola_new();
//void eliminar_cola_new();

// Planificador Corto Plazo
void iniciar_planificador_corto_plazo();
void estado_ready(void *data);
void estado_exec(void *data);
void estado_blocked(void *data);

void agregar_proceso_a_ready(t_pcb *proceso);
void iniciar_cola_ready();
void eliminar_cola_ready();
void ejecutar_proceso();
//bool hay_proceso_en_ejecucion();
void analizar_datos(t_paquete *paquete);
void enviar_proceso_a_cpu(t_pcb *pcb, int socket_cpu_dispatch);
t_paquete *esperar_respuesta_cpu(int socket_cpu_dispatch);

void planificacionFIFO();
void planificacionSRT();

// Planificador Mediano Plazo
void agregar_proceso_a_suspended_blocked(t_pcb *proceso);
void iniciar_cola_suspended_blocked();
void iniciar_cola_suspended_ready();
void iniciar_cola_blocked();

typedef struct {
	int socket;
	uint32_t id;
} t_pid;

#endif /* PLANIFICADOR_H_ */
