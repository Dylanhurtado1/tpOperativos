#include "mmu.h"


uint32_t traducir_direccion_logica(uint32_t tabla_primer_nivel, uint32_t direccion_logica) {
	uint32_t tabla_segundo_nivel;
	uint32_t numero_de_marco;
	t_traducciones traducciones = parsear_direccion_logica(direccion_logica);

	if(tlb_hit(traducciones.numero_pagina)) {
		numero_de_marco = tlb_marco_de_pagina(traducciones.numero_pagina);
	} else {
		tabla_segundo_nivel = acceder_tablas_en_memoria(tabla_primer_nivel, traducciones.entrada_tabla_nivel_1, ACCESO_TABLA_PRIMER_NIVEL);
		numero_de_marco = acceder_tablas_en_memoria(tabla_segundo_nivel, traducciones.entrada_tabla_nivel_2, ACCESO_TABLA_SEGUNDO_NIVEL);
		tlb_agregar_entrada(traducciones.numero_pagina, numero_de_marco);
	}

	uint32_t direccion_fisica = numero_de_marco * traductor->tamanio_pagina + traducciones.desplazamiento;

	/*log_info(cpu_logger, "Tabla primer nivel = %d", tabla_primer_nivel);
	log_info(cpu_logger, "Tabla segundo nivel = %d", tabla_segundo_nivel);
	log_info(cpu_logger, "Marco de pagina = %d", numero_de_marco);
	log_info(cpu_logger, "Direccion Fisica = %d", direccion_fisica);*/

	return direccion_fisica;
}

uint32_t leer_memoria(uint32_t direccion_fisica) {
	uint32_t valor_memoria;
	t_paquete *paquete = crear_paquete(LEER_MEMORIA, buffer_vacio());

	agregar_a_paquete(paquete, &direccion_fisica, sizeof(uint32_t));
	enviar_paquete(paquete, socket_memoria);
	eliminar_paquete(paquete);

	recibir_datos(socket_memoria, &valor_memoria, sizeof(uint32_t));
	return valor_memoria;
}

void escribir_memoria(uint32_t direccion_fisica, uint32_t valor) {
	t_paquete *paquete = crear_paquete(ESCRIBIR_MEMORIA, buffer_vacio());

	agregar_a_paquete(paquete, &direccion_fisica, sizeof(uint32_t));
	agregar_a_paquete(paquete, &valor, sizeof(uint32_t));
	enviar_paquete(paquete, socket_memoria);
	eliminar_paquete(paquete);
}

t_traducciones parsear_direccion_logica(uint32_t direcion_logica) {
	t_traducciones traducciones;
	traducciones.numero_pagina =  (uint32_t)(direcion_logica / traductor->tamanio_pagina);
	traducciones.entrada_tabla_nivel_1 =  (uint32_t)(traducciones.numero_pagina / traductor->cantidad_entradas_tabla);
	traducciones.entrada_tabla_nivel_2 =  traducciones.numero_pagina % traductor->cantidad_entradas_tabla;
	traducciones.desplazamiento =  direcion_logica - traducciones.numero_pagina * traductor->tamanio_pagina;

	/*log_info(cpu_logger, "Direccion Logica = %d", direcion_logica);
	log_info(cpu_logger, "Numero de pagina = %d", traducciones.numero_pagina);
	log_info(cpu_logger, "Entrada tabla nivel 1 = %d", traducciones.entrada_tabla_nivel_1);
	log_info(cpu_logger, "Entrada tabla nivel 2 = %d", traducciones.entrada_tabla_nivel_2);
	log_info(cpu_logger, "Desplazamiento = %d", traducciones.desplazamiento);*/

	return traducciones;
}

uint32_t acceder_tablas_en_memoria(uint32_t direccion_tabla, uint32_t entrada_tabla, t_protocolo protocolo) {
	uint32_t valor_obtenido;
	t_paquete *paquete = serializar_tabla_de_acceso(direccion_tabla, entrada_tabla, protocolo);
	enviar_paquete(paquete, socket_memoria);
	eliminar_paquete(paquete);

	recibir_datos(socket_memoria, &valor_obtenido, sizeof(uint32_t));
	return valor_obtenido;
}


