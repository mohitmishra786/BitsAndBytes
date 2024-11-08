#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define BUFFER_SIZE 256  // Must be power of 2 for efficient wrapping

typedef struct {
    uint8_t data[BUFFER_SIZE];
    volatile uint8_t write_ptr;  // Volatile for interrupt safety
    volatile uint8_t read_ptr;
} RingBuffer;

void ring_buffer_init(RingBuffer* buffer) {
    buffer->write_ptr = 0;
    buffer->read_ptr = 0;
}

bool ring_buffer_write(RingBuffer* buffer, uint8_t data) {
    uint8_t next_write = (buffer->write_ptr + 1) & (BUFFER_SIZE - 1);
    
    if (next_write == buffer->read_ptr) {
        return false;  // Buffer full
    }
    
    buffer->data[buffer->write_ptr] = data;
    buffer->write_ptr = next_write;
    return true;
}

bool ring_buffer_read(RingBuffer* buffer, uint8_t* data) {
    if (buffer->read_ptr == buffer->write_ptr) {
        return false;  // Buffer empty
    }
    
    *data = buffer->data[buffer->read_ptr];
    buffer->read_ptr = (buffer->read_ptr + 1) & (BUFFER_SIZE - 1);
    return true;
}

uint8_t ring_buffer_size(RingBuffer* buffer) {
    return (buffer->write_ptr - buffer->read_ptr) & (BUFFER_SIZE - 1);
}
