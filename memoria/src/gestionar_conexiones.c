#include "gestionar_conexiones.h"

static void retardo_respuesta(uint32_t tiempo);


void procesar_conexiones(t_cliente *datos_cliente) {
	t_pcb *pcb;
	t_tabla_acceso *tabla;
	t_list *datos;
	uint32_t direccion_fisica;

	t_paquete *paquete = datos_cliente->paquete;
	switch (paquete->codigo_operacion) {
		// KERNEL
		case INICIALIZACION_DE_PROCESO:
			pcb = deserializar_pcb(paquete);
			log_info(memoria_logger, "Inicializando estructuras de PID[%d]...", pcb->id);
			swap_crear_archivo(pcb->id, pcb->tamanio_proceso);
			uint32_t tabla_primer_nivel = crear_tablas_de_paginacion(pcb->id);
			enviar_direccion_tabla_primer_nivel(datos_cliente->socket, tabla_primer_nivel);

			eliminar_pcb(pcb);
			break;
		case SUSPENSION_DE_PROCESO:
			pcb = deserializar_pcb(paquete);
			log_info(memoria_logger, "Liberando memoria de PID[%d]...", pcb->id);
			liberar_espacio_de_usuario(pcb->id);
			liberar_paginas_cargadas(pcb->id);
			informar_estado_proceso(datos_cliente->socket, PROCESO_SUSPENDIDO);

			eliminar_pcb(pcb);
			break;
		case FINALIZACION_DE_PROCESO:
			pcb = deserializar_pcb(paquete);
			log_info(memoria_logger, "Eliminando memoria de PID[%d]...", pcb->id);
			liberar_espacio_de_usuario(pcb->id);
			liberar_paginas_cargadas(pcb->id);
			swap_eliminar_archivo(pcb->id);
			informar_estado_proceso(datos_cliente->socket, PROCESO_FINALIZADO);

			eliminar_pcb(pcb);
			break;
		// CPU
		case HANDSHAKE_INICIAL:
			log_info(memoria_logger, "Memoria recibio handshake, enviando estructura traductora...");
			t_traductor *traductor = crear_traductor_direcciones(memoria_config->entradas_por_tabla, memoria_config->tamanio_pagina);
			enviar_estructura_traductora(datos_cliente->socket, traductor);

			eliminar_traductor_direcciones(traductor);
			break;
		case ACCESO_TABLA_PRIMER_NIVEL:
			log_info(memoria_logger, "Enviando tabla segundo nivel...");
			tabla = deserializar_tabla_de_acceso(paquete);
			uint32_t tabla_segundo_nivel = get_tabla_segundo_nivel(tabla->direccion, tabla->entrada);
			enviar_direccion_tabla_segundo_nivel(datos_cliente->socket, tabla_segundo_nivel);

			eliminar_tabla_de_acceso(tabla);
			break;
		case ACCESO_TABLA_SEGUNDO_NIVEL:
			log_info(memoria_logger, "Enviando numero de marco...");
			tabla = deserializar_tabla_de_acceso(paquete);
			uint32_t marco = get_marco_de_pagina(tabla->direccion, tabla->entrada);
			enviar_numero_marco_de_pagina(datos_cliente->socket, marco);

			eliminar_tabla_de_acceso(tabla);
			break;
		case LEER_MEMORIA:
			log_info(memoria_logger, "Enviando valor de memoria...");
			datos = deserealizar_paquete(paquete);
			direccion_fisica = *(uint32_t *)list_get(datos, 0);
			log_info(memoria_logger, "Direccion fisica a leer = %d", direccion_fisica);

			uint32_t valor_de_memoria = leer_memoria_principal(direccion_fisica);
			enviar_valor_leido_de_memoria(datos_cliente->socket, valor_de_memoria);

			list_destroy_and_destroy_elements(datos, free);
			break;
		case ESCRIBIR_MEMORIA:
			log_info(memoria_logger, "Escribiendo valor en memoria...");
			datos = deserealizar_paquete(paquete);
			direccion_fisica = *(uint32_t *)list_get(datos, 0);
			uint32_t valor_a_escribir = *(uint32_t *)list_get(datos, 1);
			log_info(memoria_logger, "Direccion fisica a escribir = %d", direccion_fisica);
			log_info(memoria_logger, "Valor = %d", valor_a_escribir);

			escribir_memoria_principal(direccion_fisica, valor_a_escribir);
			// TODO: ver si se escribe en memoria correctamente o no y enviar respuesta acorde a CPU
			actualizar_pagina_modificada(marco_modificado(direccion_fisica));

			list_destroy_and_destroy_elements(datos, free);
			break;
		default:
			log_error(memoria_logger, "Protocolo invalido.");
			break;
	}
	//eliminar_paquete(paquete);
}

void enviar_direccion_tabla_primer_nivel(int socket_fd, uint32_t direccion) {
	enviar_datos(socket_fd, &direccion, sizeof(uint32_t));
}

void informar_estado_proceso(int socket_fd, t_protocolo protocolo) {
	enviar_datos(socket_fd, &protocolo, sizeof(t_protocolo));
}

void enviar_estructura_traductora(int socket_fd, t_traductor *traductor) {
	t_paquete *paquete = serializar_traductor(traductor, HANDSHAKE_INICIAL);
	retardo_respuesta(memoria_config->retardo_memoria);
	enviar_paquete(paquete, socket_fd);
	eliminar_paquete(paquete);
}

void enviar_direccion_tabla_segundo_nivel(int socket_fd, uint32_t direccion) {
	retardo_respuesta(memoria_config->retardo_memoria);
	enviar_datos(socket_fd, &direccion, sizeof(uint32_t));
}

void enviar_numero_marco_de_pagina(int socket_fd, uint32_t numero) {
	retardo_respuesta(memoria_config->retardo_memoria);
	enviar_datos(socket_fd, &numero, sizeof(uint32_t));
}

void enviar_valor_leido_de_memoria(int socket_fd, uint32_t valor) {
	retardo_respuesta(memoria_config->retardo_memoria);
	enviar_datos(socket_fd, &valor, sizeof(uint32_t));
}


static void retardo_respuesta(uint32_t tiempo) {
	usleep(tiempo * 1000);
}

