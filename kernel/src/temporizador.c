#include "temporizador.h"

void actualizar_timer(void *data);

uint32_t tiempo = 0;
pthread_mutex_t mutex_timer;
pthread_t th_timer;

void iniciar_timer() {
	pthread_mutex_init(&mutex_timer, NULL);
	pthread_create(&th_timer, NULL, (void *)actualizar_timer, NULL);//q es esto??
	pthread_detach(th_timer);
}

uint32_t get_tiempo_actual() {//el tiempo actual
	pthread_mutex_lock(&mutex_timer);
	uint32_t tiempo_actual = tiempo * 1000;
	pthread_mutex_unlock(&mutex_timer);
	return tiempo_actual;
}


void actualizar_timer(void *data) {
	while(1) {
		sleep(1);
		pthread_mutex_lock(&mutex_timer);
		tiempo++;
		pthread_mutex_unlock(&mutex_timer);
	}
}
