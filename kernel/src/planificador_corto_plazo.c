#include "planificador.h"
#include "kernel.h"

void eliminar_proceso_cola_ready(t_pcb *proceso);
void eliminar_proceso(t_pcb *proceso);
//void enviar_interrupcion_a_cpu(int socket_fd);

t_queue *cola_ready;
t_queue *cola_new;
t_queue *cola_exec;
bool exec = false;

//extern int socket_cpu_interrupt;
extern int socket_cpu_dispatch;
extern t_log *kernel_logger;
extern uint32_t procesos_admitidos_en_ready;

void agregar_proceso_a_ready(t_pcb *proceso) {
	queue_push(cola_ready, proceso);
}

void ejecutar_proceso() {
	if(hay_proceso_en_ejecucion()) {
		log_info(kernel_logger, "Hay un proceso en ejecucion");
		return;
	}
	t_pcb *proceso = queue_pop(cola_ready);
	t_paquete *paquete = serializar_pcb(proceso, PCB);
	enviar_paquete(paquete, socket_cpu_dispatch);
	eliminar_paquete(paquete);
	eliminar_proceso(proceso);
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
}



void enviar_interrupcion_a_cpu(int socket_fd){
	uint32_t nada = 0;
	t_pcb *pcbProceso;
	t_paquete *paquete = crear_paquete(DESALOJAR_PROCESO, buffer_vacio());
	agregar_a_paquete(paquete, &nada, sizeof(uint32_t));           //Envia una interrupcion, igual que con memoria no tiene que enviar datos
	enviar_paquete(paquete, socket_fd);
	eliminar_paquete(paquete);

	//recibir_datos(socket_fd, &pcbProceso, sizeof(t_pcb));
	//log_info(kernel_logger, "PCB del proceso en ejecucion desalojado recibido");

}
*/




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
	iniciar_hilo_cola_ready();
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




































