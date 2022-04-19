#include "buffer.h"

t_buffer *buffer_vacio(void) {
    t_buffer *buffer = (t_buffer *)malloc(sizeof(t_buffer));
    buffer->size = 0;
    buffer->stream = NULL;
    return buffer;
}

t_buffer *crear_buffer(void *stream, uint32_t size) {
    t_buffer *buffer = (t_buffer *)malloc(sizeof(t_buffer));
    buffer->size = size;
    buffer->stream = stream;
    return buffer;
}

void agregar_a_buffer(t_buffer *buffer, void *src, uint32_t size) {
    buffer->stream = realloc(buffer->stream, buffer->size + size);
    memcpy(buffer->stream + buffer->size, src, size);
    buffer->size += size;
}

void quitar_de_buffer(t_buffer *buffer, void *dest, uint32_t size) {
    memcpy(dest, buffer->stream, size);
    buffer->size -= size;
    memmove(buffer->stream, buffer->stream + size, buffer->size);
    buffer->stream = realloc(buffer->stream, buffer->size);
}

void eliminar_buffer(t_buffer *buffer) {
    free(buffer->stream);
    free(buffer);
}
