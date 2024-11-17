#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

typedef struct {
    char* buffer;
    size_t size;
    size_t position;
    int fd;
} CustomBuffer;

CustomBuffer* create_buffer(int fd) {
    CustomBuffer* buf = malloc(sizeof(CustomBuffer));
    if (!buf) return NULL;
    
    buf->buffer = malloc(BUFFER_SIZE);
    if (!buf->buffer) {
        free(buf);
        return NULL;
    }
    
    buf->size = BUFFER_SIZE;
    buf->position = 0;
    buf->fd = fd;
    return buf;
}

int flush_buffer(CustomBuffer* buf) {
    if (buf->position == 0) return 0;
    
    ssize_t written = write(buf->fd, buf->buffer, buf->position);
    if (written == -1) return -1;
    
    buf->position = 0;
    return 0;
}

int write_to_buffer(CustomBuffer* buf, const char* data, size_t len) {
    while (len > 0) {
        size_t available = buf->size - buf->position;
        size_t to_write = len < available ? len : available;
        
        memcpy(buf->buffer + buf->position, data, to_write);
        buf->position += to_write;
        data += to_write;
        len -= to_write;
        
        if (buf->position == buf->size) {
            if (flush_buffer(buf) == -1) return -1;
        }
    }
    return 0;
}

void destroy_buffer(CustomBuffer* buf) {
    if (buf) {
        free(buf->buffer);
        free(buf);
    }
}

int main() {
    CustomBuffer* buf = create_buffer(STDOUT_FILENO);
    if (!buf) {
        fprintf(stderr, "Failed to create buffer\n");
        return 1;
    }
    
    const char* message = "Hello, buffered world!\n";
    if (write_to_buffer(buf, message, strlen(message)) == -1) {
        fprintf(stderr, "Write failed\n");
        destroy_buffer(buf);
        return 1;
    }
    
    flush_buffer(buf);
    destroy_buffer(buf);
    return 0;
}
