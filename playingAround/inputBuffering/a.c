#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>

#define PAGE_SIZE 4096
#define UART_BASE 0x1000
#define UART_DATA (0x00)
#define UART_STATUS (0x01)
#define UART_CONTROL (0x02)
#define UART_DATA_READY 0x80
#define UART_OVERRUN 0x40
#define BUFFER_SIZE 256

typedef struct {
    uint8_t data[BUFFER_SIZE];
    volatile uint8_t write_ptr;
    volatile uint8_t read_ptr;
} RingBuffer;

static RingBuffer uart_rx_buffer;
volatile void *uart_base;
int fd; // Declare fd here

void uart_isr(int signum);
void uart_init(void);
bool uart_getchar(uint8_t *data, uint16_t timeout_ms);

void ring_buffer_init(RingBuffer* buffer) {
    buffer->write_ptr = 0;
    buffer->read_ptr = 0;
}

bool ring_buffer_write(RingBuffer* buffer, uint8_t data) {
    uint8_t next_write = (buffer->write_ptr + 1) % BUFFER_SIZE;
    if (next_write == buffer->read_ptr)
        return false;
    buffer->data[buffer->write_ptr] = data;
    buffer->write_ptr = next_write;
    return true;
}

bool ring_buffer_read(RingBuffer* buffer, uint8_t* data) {
    if (buffer->read_ptr == buffer->write_ptr)
        return false;
    *data = buffer->data[buffer->read_ptr];
    buffer->read_ptr = (buffer->read_ptr + 1) % BUFFER_SIZE;
    return true;
}

void uart_isr(int signum) {
    volatile uint8_t *uart_status = (uint8_t*)((uint8_t*)uart_base + UART_STATUS);
    volatile uint8_t *uart_data = (uint8_t*)((uint8_t*)uart_base + UART_DATA);

    if (*uart_status & UART_DATA_READY) {
        uint8_t received_data = *uart_data;
        if (!ring_buffer_write(&uart_rx_buffer, received_data)) {
            fprintf(stderr, "UART receive buffer overflow!\n");
        }
    }
}

void uart_init(void) {
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("Can't open /dev/mem");
        exit(EXIT_FAILURE);
    }

    uart_base = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, UART_BASE & ~(PAGE_SIZE - 1));
    if (uart_base == MAP_FAILED) {
        perror("Can't map memory");
        close(fd);
        exit(EXIT_FAILURE);
    }

    ring_buffer_init(&uart_rx_buffer);

    struct sigaction sa;
    sa.sa_handler = uart_isr;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("Error: cannot handle SIGALRM");
        exit(EXIT_FAILURE);
    }

    ((volatile uint8_t*)uart_base)[UART_CONTROL] = 0x0B;
}

bool uart_getchar(uint8_t *data, uint16_t timeout_ms) {
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

    munmap((void*)uart_base, PAGE_SIZE);
    close(fd);
    return 0;
}
