#include "planificador.h"
#include "kernel.h"

void eliminar_proceso_cola_ready(t_pcb *proceso);
void eliminar_proceso(t_pcb *proceso);
void enviar_interrupcion_a_cpu(int socket_fd);
void enviar_proceso_a_memoria(t_pcb *pcb, int socket_memoria);
t_paquete *esperar_respuesta_memoria(int socket_memoria);

t_queue *cola_ready;
t_queue *cola_new;
t_queue *cola_exec;
bool exec = false;

extern int socket_cpu_interrupt;
extern int socket_cpu_dispatch;
extern int socket_memoria;//esto para conectarse con memoria
extern t_log *kernel_logger;
extern uint32_t procesos_admitidos_en_ready;

void agregar_proceso_a_ready(t_pcb *proceso) {
	queue_push(cola_ready, proceso);
	// TODO: agregar condicion para saber si es SRT (la interrupcion solo se envia en SRT)
	enviar_interrupcion_a_cpu(socket_cpu_interrupt);
}

void ejecutar_proceso() {
	if(hay_proceso_en_ejecucion()) {
		log_info(kernel_logger, "Hay un proceso en ejecucion");
		return;
	}
	t_pcb *proceso = queue_pop(cola_ready);
	enviar_proceso_a_cpu(proceso, socket_cpu_dispatch);//todos los procesos son enviados por dispatch
	//eliminar_proceso(proceso);

	t_protocolo protocolo;
	do {
		t_paquete *paquete = esperar_respuesta_cpu(socket_cpu_dispatch);
		protocolo = paquete->codigo_operacion;
		analizar_datos(paquete);
		eliminar_paquete(paquete);
	} while (protocolo != FINALIZAR_PROCESO);
	procesos_admitidos_en_ready--;
}

bool hay_proceso_en_ejecucion() {
	return exec;
}

void iniciar_cola_ready() {
	cola_ready = queue_create();
}


void iniciar_cola_exec(){
	cola_exec = queue_create();
}

void eliminar_cola_ready() {
	queue_destroy_and_destroy_elements(cola_ready, (void *)eliminar_proceso_cola_ready);
}

void eliminar_proceso_cola_ready(t_pcb *proceso) {
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}

void eliminar_proceso(t_pcb *proceso) {
	list_destroy_and_destroy_elements(proceso->instrucciones, free);
	free(proceso);
}

void analizar_datos(t_paquete *paquete) {
	t_list *datos = deserealizar_paquete(paquete);
	t_pcb *pcb;
	switch(paquete->codigo_operacion) {
		case BLOQUEAR_PROCESO:
			log_info(kernel_logger, "Proceso ejecuto IO, enviando a cola de bloqueo...");
			uint32_t *tiempo_bloqueo = (uint32_t *)list_remove(datos, list_size(datos) - 1);
			pcb = deserializar_pcb(datos, kernel_logger);
			agregar_proceso_a_blocked(pcb);//se agrega a la cola de bloqueados
			log_info(kernel_logger, "Tiempo bloqueo = %d", *tiempo_bloqueo);
			usleep(*tiempo_bloqueo * 1000);
			log_info(kernel_logger, "Finalizo bloqueo, enviando proceso a CPU...");
			enviar_proceso_a_cpu(pcb, socket_cpu_dispatch);
			eliminar_proceso(pcb);
			free(tiempo_bloqueo);
			break;
		case FINALIZAR_PROCESO:
			log_info(kernel_logger, "Proceso finalizado, liberando recursos a memoria...");//todavia no implementado
			pcb = deserializar_pcb(datos, kernel_logger);
			enviar_proceso_a_memoria(pcb, socket_memoria); //falta la parte de memoria :(
			esperar_respuesta_memoria(socket_memoria);
			log_info(kernel_logger,"Aviso a consola que finaliza proceso :)");
			//mandar un mensaje a memoria
			//espera respuesta de memoria
			//envia mensaje a consola
			eliminar_proceso(pcb);
			break;
		case PROCESO_DESALOJADO:
			log_info(kernel_logger, "Proceso desalojado por interrupcion, seleccionar siguiente proceso a ejecutar...");
			pcb = deserializar_pcb(datos, kernel_logger);
			usleep(20000); // <-- Solo para debug
			log_info(kernel_logger, "Se envia siguiente proceso...");
			enviar_proceso_a_cpu(pcb, socket_cpu_dispatch);
			eliminar_proceso(pcb);
			break;
	}
	list_destroy_and_destroy_elements(datos, free);
}

void enviar_proceso_a_cpu(t_pcb *pcb, int socket_cpu_dispatch) {
	t_paquete *paquete = serializar_pcb(pcb, PCB);
	enviar_paquete(paquete, socket_cpu_dispatch);
	eliminar_paquete(paquete);
}

void enviar_proceso_a_memoria(t_pcb *pcb, int socket_memoria) {
	t_paquete *paquete = serializar_pcb(pcb, PCB);
	enviar_paquete(paquete, socket_memoria);
	eliminar_paquete(paquete);
}// esta funcion para el mensaje a memoria


t_paquete *esperar_respuesta_cpu(int socket_cpu_dispatch) {
	return recibir_paquete(socket_cpu_dispatch);
}

t_paquete *esperar_respuesta_memoria(int socket_memoria) {//nose si seria con dispath
	return recibir_paquete(socket_memoria);
}

bool hay_procesos_en_ready(){
	int cantidadReady = queue_size(cola_ready);
	return cantidadReady > 0;
}

/*
void agregar_proceso_a_ready_SRT(t_pcb *proceso){
	proceso = queue_pop(cola_new);
	queue_push(cola_ready, proceso);
	enviar_interrupcion_a_cpu(socket_cpu_interrupt);
	procesos_admitidos_en_ready++;
}*/



void enviar_interrupcion_a_cpu(int socket_fd){
	log_info(kernel_logger, "Enviando interrupcion de desalojo");
	t_paquete *paquete = crear_paquete(DESALOJAR_PROCESO, buffer_vacio());
	uint32_t nada = 0;
	agregar_a_paquete(paquete, &nada, sizeof(uint32_t));
	enviar_paquete(paquete, socket_fd);
	eliminar_paquete(paquete);
}




//No es que este mal, pero hay que tratarlo con hilos en cada cola porque sino ante alguna operacion o interrupcion o etc etc bloqueas TODA la funcion
//de planificar, y no es la idea. la idea es trabajarlo por colas separadas y que puedas bloquearlas con semaforos (osea sincronizarlas) ante alguna operacion / etc..




/*
void planificacionFIFO(){//ESTA ES UNA IMPLEMENTACION CON LISTAS, HAY QUE DEFINIR COLAS O LISTAS, PERO ES
	//UN APROXIMADO A LO QUE PENSE, NO ESTA TERMINADO
	    int tamanioReady = 5;//SUPONIENDO HAY 5 procesos en ready, nose si estoy haciendolo bien xD
		int tamanioNew;
		int grado_multitarea = 2;//cantidad de procesos que pueden estar ejecutando
		int ejecutando = 1;
		t_list* procesosReady;
		t_list* procesosExec;
	log_info(kernel_logger,"Hilo de planificador FIFO iniciado");

	while(true){
		//if(hay_procesos_en_ready() && !hayUnProcesoEnEjecucion()){
		//	ejecutar_proceso();
		//el pequeño fifo xD
			if (tamanioReady > 0){ //si hay procesos en ready
			if (ejecutando<grado_multitarea){//cant de procesos ejecutando < grado que permite
			int i=0;//y con este codigo todos los tripulantes de rdy pasar a exec
			while(ejecutando<grado_multitarea && (tamanioReady-i)>0){//cant de procesos en rdy > 0
			t_pcb* primerProceso= list_get(procesosReady,0);
			list_remove(procesosReady,0);
			list_add(procesosExec,primerProceso);//POR AHORA EL PANIF ES EL UNICO QUE MANEJA ESTA LISTA
			i++;
			ejecutando++;
			log_info(kernel_logger,"Se pasaron algunos procesos de ready a Exec");
			}
			}
			}
		}
}

void planificacionSRT(){
	log_info(kernel_logger,"Hilo de planificador SRT iniciado");

	while(true){
		//if(hay_procesos_en_ready()){
			//ejecutar_proceso();
		}
	}


void inicio_planificacion(){
	log_info(kernel_logger, "Iniciando planificacion");
	if(strcmp(kernel_config->algoritmo_planificacion,"FIFO") == 0){
	log_info(kernel_logger,"Algoritmo de planificacion leido: FIFO");
	pthread_t hilo_planificacion_FIFO;
	pthread_create(&hilo_planificacion_FIFO, NULL, (void*) planificacionFIFO, NULL);
	pthread_detach(hilo_planificacion_FIFO);
	}
	else {
	log_info(kernel_logger,"Algoritmo de planificacion leido: SRT");
	pthread_t hilo_planificacion_SRT;
	pthread_create(&hilo_planificacion_SRT, NULL, (void*) planificacionSRT, NULL);
	pthread_detach(hilo_planificacion_SRT);
	}
}


*/


void inicio_estructuras_planificacion(){

	//Creo las colas

	iniciar_cola_new();
	iniciar_cola_ready();
	iniciar_cola_exec();
	iniciar_cola_blocked();
	iniciar_cola_suspended_blocked();
	iniciar_cola_suspended_ready();
	iniciar_cola_fin(); //Ver si es necesario

    //iniiciar semaforos
	pthread_mutex_init(&planificador_mutex_new,NULL);
	pthread_mutex_init(&planificador_mutex_ready,NULL);
	pthread_mutex_init(&planificador_mutex_exec,NULL);
	pthread_mutex_init(&planificador_mutex_blocked,NULL);
	pthread_mutex_init(&planificador_mutex_suspended_blocked,NULL);
	pthread_mutex_init(&planificador_mutex_suspended_ready,NULL);
	pthread_mutex_init(&planificador_mutex_fin,NULL);

}





void inicio_planificacion(){
	log_info(kernel_logger, "Iniciando planificacion");
	pthread_t hilo_planificacion;
	pthread_create(&hilo_planificacion, NULL, (void*) iniciar_colas_de_planificacion, NULL);
	pthread_detach(hilo_planificacion);
}



void iniciar_colas_de_planificacion() {
	iniciar_hilo_cola_ready();//cada hilo es una cola
	iniciar_hilo_cola_blocked();
	iniciar_hilo_cola_suspended_blocked();
	iniciar_hilo_cola_suspended_ready();
}




void iniciar_hilo_cola_exec(){
	pthread_t hilo = (pthread_t)malloc(sizeof(pthread_t));
	pthread_create(&hilo ,NULL,(void*) planificar_cola_exec,NULL);
	pthread_detach(hilo);
}


void iniciar_hilo_cola_ready(){
	pthread_t hilo = (pthread_t)malloc(sizeof(pthread_t));
	pthread_create(&hilo , NULL,(void*) planificar_cola_ready,NULL);
	pthread_detach(hilo);
}


void iniciar_hilo_cola_blocked(){
	pthread_t hilo = (pthread_t)malloc(sizeof(pthread_t));
	pthread_create(&hilo , NULL,(void*) planificar_cola_block,NULL);
	pthread_detach(hilo);
}

void iniciar_hilo_cola_suspended_blocked(){
	pthread_t hilo = (pthread_t)malloc(sizeof(pthread_t));
	pthread_create(&hilo , NULL,(void*) planificar_cola_suspended_blocked,NULL);
	pthread_detach(hilo);
}


void iniciar_hilo_cola_suspended_ready(){
	pthread_t hilo = (pthread_t)malloc(sizeof(pthread_t));
	pthread_create(&hilo , NULL,(void*) planificar_cola_suspended_ready,NULL);
	pthread_detach(hilo);
}


//En ready va la logica de fifo o srt, y asi con las demas

void planificar_cola_ready(){
	log_info(kernel_logger, "Se creo el hilo para la cola de ready satisfactoriamente");
}

void planificar_cola_block(){
	log_info(kernel_logger, "Se creo el hilo para la cola de block satisfactoriamente");
}

void planificar_cola_suspended_blocked(){ //Ver como tratarlo en block proque en el enunciado dice que es una sola cola de bloqueados
	log_info(kernel_logger, "Se creo el hilo para la cola de suspended-blocked satisfactoriamente");
}

void planificar_cola_suspended_ready(){ //Lo mismo para suspendido-bloqueado
	log_info(kernel_logger, "Se creo el hilo para la cola de suspended-ready satisfactoriamente");
}

void planificar_cola_exec(){
	log_info(kernel_logger, "Se creo el hilo para la cola de exec satisfactoriamente");
}




































