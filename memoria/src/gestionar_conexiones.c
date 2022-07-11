#include "gestionar_conexiones.h"

static void retardo_respuesta(uint32_t tiempo);


void procesar_conexiones(t_cliente *datos_cliente) {//aca recibe paquetes de los demas modulos, puede ser kernel, cpu
	t_pcb *pcb;
	t_tabla_acceso *tabla;
	t_list *datos;
	uint32_t direccion_fisica;

	t_paquete *paquete = datos_cliente->paquete;
	switch (paquete->codigo_operacion) {
		// KERNEL
		case INICIALIZACION_DE_PROCESO://de kernel recibe para iniciar las estructuras
			pcb = deserializar_pcb(paquete);
			log_info(memoria_logger, "Inicializando estructuras de PID[%d]...", pcb->id);
			swap_crear_archivo(pcb->id, pcb->tamanio_proceso);//se crea el archivo swap
			uint32_t tabla_primer_nivel = crear_tablas_de_paginacion(pcb->id);//devuelve una tabla que es un int
			enviar_direccion_tabla_primer_nivel(datos_cliente->socket, tabla_primer_nivel);//se lo envia a kernel, un numero es la tabla

			eliminar_pcb(pcb);
			break;
		case SUSPENSION_DE_PROCESO://recibe de kernel este protocolo, y se debe liberar de memoria, y se escribe en el archivo swap
			pcb = deserializar_pcb(paquete);
			log_info(memoria_logger, "Liberando memoria de PID[%d]...", pcb->id);
			liberar_espacio_de_usuario(pcb->id);//pone marcos libres en el bitmap
			liberar_paginas_cargadas(pcb->id);//se aplica cambio a todas las paginas, con P = 0
			informar_estado_proceso(datos_cliente->socket, PROCESO_SUSPENDIDO);//se lo envia a kernel informando

			eliminar_pcb(pcb);
			break;
		case FINALIZACION_DE_PROCESO://recibe de kernel este protocolo
			pcb = deserializar_pcb(paquete);
			log_info(memoria_logger, "Eliminando memoria de PID[%d]...", pcb->id);//imprime
			liberar_espacio_de_usuario(pcb->id);//pone marcos libres en el bitmap
			//liberar_paginas_cargadas(pcb->id); TODO: es necesario?
			swap_eliminar_archivo(pcb->id);//se elimina el archivo swap de ese proceso
			informar_estado_proceso(datos_cliente->socket, PROCESO_FINALIZADO);//se manda a kernel, para que kernel le avise a consola que finalice

			eliminar_pcb(pcb);
			break;
		// CPU ACA VIENE LO IMPORTANTE
		case HANDSHAKE_INICIAL://Recibe handshake de cpu
			log_info(memoria_logger, "Memoria recibio handshake, enviando estructura traductora...");
			t_traductor *traductor = crear_traductor_direcciones(memoria_config->entradas_por_tabla, memoria_config->tamanio_pagina);
			enviar_estructura_traductora(datos_cliente->socket, traductor);//le envia a cpu el paquete con el traductor, tamanio pagina y cant entradas

			eliminar_traductor_direcciones(traductor);
			break;
		case ACCESO_TABLA_PRIMER_NIVEL://devuelve el numero de tabla de 2do nivel
			log_info(memoria_logger, "Enviando tabla segundo nivel...");
			tabla = deserializar_tabla_de_acceso(paquete);//devuelve algo t_tabla, direccion y entrada
			uint32_t tabla_segundo_nivel = get_tabla_segundo_nivel(tabla->direccion, tabla->entrada);//segun direccion y entrada, devuelve el numero de 2 tabla
			enviar_direccion_tabla_segundo_nivel(datos_cliente->socket, tabla_segundo_nivel);//se la mando a cpu

			eliminar_tabla_de_acceso(tabla);
			break;
		case ACCESO_TABLA_SEGUNDO_NIVEL://devuelve el numero de marco
			log_info(memoria_logger, "Enviando numero de marco...");
			tabla = deserializar_tabla_de_acceso(paquete);
			uint32_t marco = get_marco_de_pagina(tabla->direccion, tabla->entrada);//Devuelve marco segun direccion y entrada
			enviar_numero_marco_de_pagina(datos_cliente->socket, marco);//se lo manda a cpu, el numero de marco

			eliminar_tabla_de_acceso(tabla);
			break;
		case LEER_MEMORIA://devuelve el valor q se encuentre en la posicion pedida
			log_info(memoria_logger, "Enviando valor de memoria...");
			datos = deserealizar_paquete(paquete);
			direccion_fisica = *(uint32_t *)list_get(datos, 0);//obtiene la pos 0 de la lista de datos
			log_info(memoria_logger, "Direccion fisica a leer = %d", direccion_fisica);

			uint32_t valor_de_memoria = leer_memoria_principal(direccion_fisica);//lee en memoria con la dir fisica
			enviar_valor_leido_de_memoria(datos_cliente->socket, valor_de_memoria);//se lo envia a cpu

			list_destroy_and_destroy_elements(datos, free);
			break;
		case ESCRIBIR_MEMORIA://escribir lo indicado en la pos pedida y retornar OK
			log_info(memoria_logger, "Escribiendo valor en memoria...");
			datos = deserealizar_paquete(paquete);
			direccion_fisica = *(uint32_t *)list_get(datos, 0);//obtiene la pos 0 de datos, es un numero CUANDO SE HACE LIST_ADD(DATOS)
			uint32_t valor_a_escribir = *(uint32_t *)list_get(datos, 1);//pos 2 es el valor a escribir, 2do patametro
			log_info(memoria_logger, "Direccion fisica a escribir = %d", direccion_fisica);
			log_info(memoria_logger, "Valor = %d", valor_a_escribir);

			escribir_memoria_principal(direccion_fisica, valor_a_escribir);//se escribe con un memcpy
			// TODO: ver si se escribe en memoria correctamente o no y enviar respuesta acorde a CPU
			actualizar_pagina_modificada(marco_modificado(direccion_fisica));//solo se pone Marcon con M = 1

			list_destroy_and_destroy_elements(datos, free);
			break;
		default:
			log_error(memoria_logger, "Protocolo invalido.");
			break;
	}
	eliminar_paquete(paquete);
}

void enviar_direccion_tabla_primer_nivel(int socket_fd, uint32_t direccion) {//se envia por socket la direccion
	enviar_datos(socket_fd, &direccion, sizeof(uint32_t));
}

void informar_estado_proceso(int socket_fd, t_protocolo protocolo) {//le envia a ese socket datos segun el protocolo
	enviar_datos(socket_fd, &protocolo, sizeof(t_protocolo));
}

void enviar_estructura_traductora(int socket_fd, t_traductor *traductor) {//se manda a cpu
	t_paquete *paquete = serializar_traductor(traductor, HANDSHAKE_INICIAL);
	retardo_respuesta(memoria_config->retardo_memoria);//viene en el config
	enviar_paquete(paquete, socket_fd);//lo envia a cpu
	eliminar_paquete(paquete);
}

void enviar_direccion_tabla_segundo_nivel(int socket_fd, uint32_t direccion) {
	retardo_respuesta(memoria_config->retardo_memoria);
	enviar_datos(socket_fd, &direccion, sizeof(uint32_t));
}

void enviar_numero_marco_de_pagina(int socket_fd, uint32_t numero) {//se manda el numero de marco
	retardo_respuesta(memoria_config->retardo_memoria);
	enviar_datos(socket_fd, &numero, sizeof(uint32_t));
}

void enviar_valor_leido_de_memoria(int socket_fd, uint32_t valor) {//se manda con un retardo
	retardo_respuesta(memoria_config->retardo_memoria);
	enviar_datos(socket_fd, &valor, sizeof(uint32_t));
}


static void retardo_respuesta(uint32_t tiempo) {//se queda esperando un tiempo por el config
	usleep(tiempo * 1000);
}

