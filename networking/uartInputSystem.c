#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define UART_DATA    (*((volatile uint8_t*)0x1000))
#define UART_STATUS  (*((volatile uint8_t*)0x1001))
#define UART_CONTROL (*((volatile uint8_t*)0x1002))

#define UART_DATA_READY 0x80
#define UART_OVERRUN   0x40

#define BUFFER_SIZE 256

typedef struct {
    uint8_t data[BUFFER_SIZE];
    volatile uint8_t write_ptr;
    volatile uint8_t read_ptr;
} RingBuffer;

static RingBuffer uart_rx_buffer;

void ring_buffer_init(RingBuffer* buffer) {
    buffer->write_ptr = 0;
    buffer->read_ptr = 0;
}

bool ring_buffer_write(RingBuffer* buffer, uint8_t data) {
    uint8_t next_write = (buffer->write_ptr + 1) % BUFFER_SIZE;
    
    if (next_write == buffer->read_ptr) {
        return false;
    }
    
    buffer->data[buffer->write_ptr] = data;
    buffer->write_ptr = next_write;
    return true;
}

bool ring_buffer_read(RingBuffer* buffer, uint8_t* data) {
    if (buffer->read_ptr == buffer->write_ptr) {
        return false;
    }
    
    *data = buffer->data[buffer->read_ptr];
    buffer->read_ptr = (buffer->read_ptr + 1) % BUFFER_SIZE;
    return true;
}

void uart_isr(int signum) {
    uint8_t status = UART_STATUS;
    
    if (status & UART_DATA_READY) {
        uint8_t data = UART_DATA;
        if (!ring_buffer_write(&uart_rx_buffer, data)) {
            fprintf(stderr, "UART buffer overflow!\n");
        }
    }
}

void uart_init(void) {
    ring_buffer_init(&uart_rx_buffer);
    signal(SIGALRM, uart_isr);
    UART_CONTROL = 0x0B;  
}

bool uart_getchar(uint8_t* data, uint16_t timeout_ms) {
    alarm(timeout_ms / 1000);
    while (timeout_ms--) {
        if (ring_buffer_read(&uart_rx_buffer, data)) {
            alarm(0);
            return true;
        }
        usleep(1000);
    }
    alarm(0);
    return false;
}

int main() {
    uart_init();

    printf("UART initialized. Waiting for data...\n");

    while (1) {
        uint8_t received;
        if (uart_getchar(&received, 1000)) {
            printf("Received: %c\n", received);
        } else {
            printf("Timeout: No data received in 1 second.\n");
        }
    }

    return 0;
}
