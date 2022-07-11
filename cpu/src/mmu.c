#include "mmu.h"

static t_traducciones parsear_direccion_logica(uint32_t direcion_logica);
static uint32_t acceder_tablas_en_memoria(uint32_t direccion_tabla, uint32_t entrada_tabla, t_protocolo protocolo);


uint32_t mmu_traducir_direccion_logica(uint32_t tabla_primer_nivel, uint32_t direccion_logica) {//aca viene lo chido
	uint32_t tabla_segundo_nivel;//numero de la tabla de 2do nivel
	uint32_t numero_de_marco;//lo obtengo con la tabla de 2do nivel
	t_traducciones traducciones = parsear_direccion_logica(direccion_logica);//aca la dir logica la transforma en traduccion
	//con los calculos que viene en el enunciado

	if(tlb_hit(traducciones.numero_pagina)) {//si ya está en la tlb, porque la tlb tiene numero de marco y pagina
		numero_de_marco = tlb_marco_de_pagina(traducciones.numero_pagina);//con el numero de pagina, busca el marco correspondiente
	} else {//si no esta en la tlb,accedo a la tabla de segundo nivel
		tabla_segundo_nivel = acceder_tablas_en_memoria(tabla_primer_nivel, traducciones.entrada_tabla_nivel_1, ACCESO_TABLA_PRIMER_NIVEL);//numero de tabla de primer nivel, y la entrada que fue calculada en el parseo,recibo desde memoria el valor de la 2da tabla
		numero_de_marco = acceder_tablas_en_memoria(tabla_segundo_nivel, traducciones.entrada_tabla_nivel_2, ACCESO_TABLA_SEGUNDO_NIVEL);//vuelvo a hacer lo mismo, voy a memoria y recibo el numero de marco
		tlb_agregar_entrada(traducciones.numero_pagina, numero_de_marco);//y aca se agrega a la tlb,seguro con un algoritmo del config
	}

	uint32_t direccion_fisica = numero_de_marco * traductor->tamanio_pagina + traducciones.desplazamiento;

	return direccion_fisica;
}

uint32_t mmu_leer_memoria(uint32_t direccion_fisica) {//con la dire fisica, va a memoria
	uint32_t valor_memoria;
	t_paquete *paquete = crear_paquete(LEER_MEMORIA, buffer_vacio());//mando paquete a memoria, con protocolo LEER

	agregar_a_paquete(paquete, &direccion_fisica, sizeof(uint32_t));//agrego la dire fisica al paquete
	enviar_paquete(paquete, socket_memoria);//mando a memoria
	eliminar_paquete(paquete);//se elimina

	recibir_datos(socket_memoria, &valor_memoria, sizeof(uint32_t));//espero respuesta de memoria, memoria lo responde con valor
	return valor_memoria;
}

void mmu_escribir_memoria(uint32_t direccion_fisica, uint32_t valor) {//en la dir fisica, escribo ese valor
	t_paquete *paquete = crear_paquete(ESCRIBIR_MEMORIA, buffer_vacio());//protocolo ESCRIBIR

	agregar_a_paquete(paquete, &direccion_fisica, sizeof(uint32_t));//agrego al paquete la dir fisica
	agregar_a_paquete(paquete, &valor, sizeof(uint32_t));//el valor a escribir en memoria
	enviar_paquete(paquete, socket_memoria);//se lo mando a memoria
	eliminar_paquete(paquete);//lo elimino
}


static t_traducciones parsear_direccion_logica(uint32_t direcion_logica) {//la dir logica es un entero, lo transformo en el formato de dir logica
	t_traducciones traducciones;
	traducciones.numero_pagina =  (uint32_t)(direcion_logica / traductor->tamanio_pagina);
	traducciones.entrada_tabla_nivel_1 =  (uint32_t)(traducciones.numero_pagina / traductor->cantidad_entradas_tabla);
	traducciones.entrada_tabla_nivel_2 =  traducciones.numero_pagina % traductor->cantidad_entradas_tabla;
	traducciones.desplazamiento =  direcion_logica - traducciones.numero_pagina * traductor->tamanio_pagina;

	return traducciones;//t_traduccion ya es el formato de dir logica
}

static uint32_t acceder_tablas_en_memoria(uint32_t direccion_tabla, uint32_t entrada_tabla, t_protocolo protocolo) {//con la tabla de primer nivel la entrada calculada y protocolo
	t_tabla_acceso *tabla_acceso = crear_tabla_de_acceso(direccion_tabla, entrada_tabla);//es un parseo, con dir_tabla y entrada_tabla
	uint32_t valor_obtenido;
	t_paquete *paquete = serializar_tabla_de_acceso(tabla_acceso, protocolo);//porque con esa tabla se la mando a memoria en un paquete y segun el protocolo devuelve #marco o #2databla
	enviar_paquete(paquete, socket_memoria);//mando a memoria
	eliminar_paquete(paquete);//se elimina paquete
	eliminar_tabla_de_acceso(tabla_acceso);//se elimina tabla de acceso, porque?

	recibir_datos(socket_memoria, &valor_obtenido, sizeof(uint32_t));//espera respuesta de memoria, con un entero
	return valor_obtenido;//este seria el valor recibido por memoria, el numero de la tabla de 2do nivel o #marco
}


