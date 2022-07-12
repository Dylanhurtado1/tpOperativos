#ifndef CPU_H_
#define CPU_H_

#include "cpu_global.h"
#include "peticiones.h"
#include <signal.h>

#define PATH_CPU_CONFIG "/home/utnso/tp-2022-1c-lo-importante-es-aprobar/cpu/cpu.config"

t_traductor *obtener_traductor_direcciones(int socket_fd);
void realizar_handshake(int socket_fd);


#endif /* CPU_H_ */
