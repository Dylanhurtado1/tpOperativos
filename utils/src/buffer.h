#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t size;
    void *stream;
} t_buffer;

t_buffer *buffer_vacio(void);
t_buffer *crear_buffer(void *stream, uint32_t size);
void agregar_a_buffer(t_buffer *buffer, void *src, uint32_t size);
void quitar_de_buffer(t_buffer *buffer, void *dest, uint32_t size);
void eliminar_buffer(t_buffer *buffer);

#endif
