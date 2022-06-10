#include "mmu.h"


uint32_t traducir_direccion_logica(uint32_t tabla_primer_nivel, uint32_t direccion_logica) {
	t_traducciones traducciones = parsear_direccion_logica(direccion_logica);
	log_info(cpu_logger, "Tabla primer nivel = %d", tabla_primer_nivel);
	uint32_t tabla_segundo_nivel = obtener_tabla_segundo_nivel(socket_memoria, tabla_primer_nivel, traducciones.entrada_tabla_nivel_1);
	log_info(cpu_logger, "Tabla segundo nivel = %d", tabla_segundo_nivel);
	uint32_t numero_de_marco = obtener_numero_de_marco(socket_memoria, tabla_segundo_nivel, traducciones.entrada_tabla_nivel_2);
	log_info(cpu_logger, "Marco de pagina = %d", numero_de_marco);
	uint32_t direccion_fisica = numero_de_marco * traductor->tamanio_pagina + traducciones.desplazamiento;
	log_info(cpu_logger, "Direccion Fisica = %d", direccion_fisica);

	return direccion_fisica;
}

uint32_t leer_memoria(uint32_t direccion_fisica) {
	uint32_t valor_memoria = obtener_valor_de_memoria(socket_memoria, direccion_fisica);

	return valor_memoria;
}

void escribir_memoria(uint32_t direccion_fisica, uint32_t valor) {
	enviar_valor_a_memoria(socket_memoria, direccion_fisica, valor);
}

t_traducciones parsear_direccion_logica(uint32_t direcion_logica) {
	t_traducciones traducciones;
	traducciones.numero_pagina =  (uint32_t)(direcion_logica / traductor->tamanio_pagina);
	traducciones.entrada_tabla_nivel_1 =  (uint32_t)(traducciones.numero_pagina / traductor->cantidad_entradas_tabla);
	traducciones.entrada_tabla_nivel_2 =  traducciones.numero_pagina % traductor->cantidad_entradas_tabla;
	traducciones.desplazamiento =  direcion_logica - traducciones.numero_pagina * traductor->tamanio_pagina;

	log_info(cpu_logger, "Direccion Logica = %d", direcion_logica);
	log_info(cpu_logger, "Numero de pagina = %d", traducciones.numero_pagina);
	log_info(cpu_logger, "Entrada tabla nivel 1 = %d", traducciones.entrada_tabla_nivel_1);
	log_info(cpu_logger, "Entrada tabla nivel 2 = %d", traducciones.entrada_tabla_nivel_2);
	log_info(cpu_logger, "Desplazamiento = %d", traducciones.desplazamiento);

	return traducciones;
}

uint32_t obtener_tabla_segundo_nivel(int socket_memoria, uint32_t tabla_primer_nivel, uint32_t entrada_tabla_nivel_1) {
	uint32_t tabla_segundo_nivel;
	t_paquete *paquete = serializar_acceso_tabla(tabla_primer_nivel, entrada_tabla_nivel_1, ACCESO_TABLA_PRIMER_NIVEL);
	enviar_paquete(paquete, socket_memoria);
	recibir_datos(socket_memoria, &tabla_segundo_nivel, sizeof(uint32_t));
	eliminar_paquete(paquete);

	return tabla_segundo_nivel;
}

uint32_t obtener_numero_de_marco(int socket_memoria, uint32_t tabla_segundo_nivel, uint32_t entrada_tabla_nivel_2) {
	uint32_t marco_de_pagina;
	t_paquete *paquete = serializar_acceso_tabla(tabla_segundo_nivel, entrada_tabla_nivel_2, ACCESO_TABLA_SEGUNDO_NIVEL);
	enviar_paquete(paquete, socket_memoria);
	recibir_datos(socket_memoria, &marco_de_pagina, sizeof(uint32_t));
	eliminar_paquete(paquete);

	return marco_de_pagina;
}

uint32_t obtener_valor_de_memoria(int socket_memoria, uint32_t direccion_fisica) {
	uint32_t valor;
	t_paquete *paquete = crear_paquete(LEER_MEMORIA, buffer_vacio());
	agregar_a_paquete(paquete, &direccion_fisica, sizeof(uint32_t));
	enviar_paquete(paquete, socket_memoria);

	recibir_datos(socket_memoria, &valor, sizeof(uint32_t));
	eliminar_paquete(paquete);

	return valor;
}

void enviar_valor_a_memoria(int socket_memoria, uint32_t direccion_fisica, uint32_t valor) {
	t_paquete *paquete = crear_paquete(ESCRIBIR_MEMORIA, buffer_vacio());
	agregar_a_paquete(paquete, &direccion_fisica, sizeof(uint32_t));
	agregar_a_paquete(paquete, &valor, sizeof(uint32_t));
	enviar_paquete(paquete, socket_memoria);
	eliminar_paquete(paquete);
}


