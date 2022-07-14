#include "mmu.h"

static t_traducciones parsear_direccion_logica(uint32_t direcion_logica);
static uint32_t acceder_tablas_en_memoria(uint32_t direccion_tabla, uint32_t entrada_tabla, t_protocolo protocolo);


uint32_t mmu_traducir_direccion_logica(uint32_t tabla_primer_nivel, uint32_t direccion_logica) {
	uint32_t tabla_segundo_nivel;
	uint32_t numero_de_marco;
	t_traducciones traducciones = parsear_direccion_logica(direccion_logica);

	if(tlb_hit(traducciones.numero_pagina)) {
		numero_de_marco = tlb_marco_de_pagina(traducciones.numero_pagina);
	} else {
		tabla_segundo_nivel = acceder_tablas_en_memoria(tabla_primer_nivel, traducciones.entrada_tabla_nivel_1, ACCESO_TABLA_PRIMER_NIVEL);
		numero_de_marco = acceder_tablas_en_memoria(tabla_segundo_nivel, traducciones.entrada_tabla_nivel_2, ACCESO_TABLA_SEGUNDO_NIVEL);
		tlb_agregar_entrada(traducciones.numero_pagina, numero_de_marco);
		cantidad_acceso_tlb += 2;
	}

	uint32_t direccion_fisica = numero_de_marco * traductor->tamanio_pagina + traducciones.desplazamiento;

	return direccion_fisica;
}

uint32_t mmu_leer_memoria(uint32_t direccion_fisica) {
	uint32_t valor_memoria;
	t_paquete *paquete = crear_paquete(LEER_MEMORIA, buffer_vacio());

	agregar_a_paquete(paquete, &direccion_fisica, sizeof(uint32_t));
	enviar_paquete(paquete, socket_memoria);
	eliminar_paquete(paquete);

	recibir_datos(socket_memoria, &valor_memoria, sizeof(uint32_t));
	cantidad_acceso_tlb += 1;
	return valor_memoria;
}

void mmu_escribir_memoria(uint32_t direccion_fisica, uint32_t valor) {
	t_paquete *paquete = crear_paquete(ESCRIBIR_MEMORIA, buffer_vacio());

	agregar_a_paquete(paquete, &direccion_fisica, sizeof(uint32_t));
	agregar_a_paquete(paquete, &valor, sizeof(uint32_t));
	enviar_paquete(paquete, socket_memoria);
	eliminar_paquete(paquete);

	t_protocolo protocolo;
	recibir_datos(socket_memoria, &protocolo, sizeof(t_protocolo));

	if(protocolo != MEMORIA_MODIFICADA_OK) {
		log_error(cpu_logger, "No se escribio memoria correctamente");
	}
	cantidad_acceso_tlb += 1;
}


static t_traducciones parsear_direccion_logica(uint32_t direcion_logica) {
	t_traducciones traducciones;
	traducciones.numero_pagina =  (uint32_t)(direcion_logica / traductor->tamanio_pagina);
	traducciones.entrada_tabla_nivel_1 =  (uint32_t)(traducciones.numero_pagina / traductor->cantidad_entradas_tabla);
	traducciones.entrada_tabla_nivel_2 =  traducciones.numero_pagina % traductor->cantidad_entradas_tabla;
	traducciones.desplazamiento =  direcion_logica - traducciones.numero_pagina * traductor->tamanio_pagina;

	return traducciones;
}

static uint32_t acceder_tablas_en_memoria(uint32_t direccion_tabla, uint32_t entrada_tabla, t_protocolo protocolo) {
	t_tabla_acceso *tabla_acceso = crear_tabla_de_acceso(direccion_tabla, entrada_tabla);
	uint32_t valor_obtenido;
	t_paquete *paquete = serializar_tabla_de_acceso(tabla_acceso, protocolo);
	enviar_paquete(paquete, socket_memoria);
	eliminar_paquete(paquete);
	eliminar_tabla_de_acceso(tabla_acceso);

	recibir_datos(socket_memoria, &valor_obtenido, sizeof(uint32_t));
	return valor_obtenido;
}


