#ifndef TEMPORIZADOR_H_
#define TEMPORIZADOR_H_

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

void iniciar_timer();
uint32_t get_tiempo_actual();


#endif /* TEMPORIZADOR_H_ */
