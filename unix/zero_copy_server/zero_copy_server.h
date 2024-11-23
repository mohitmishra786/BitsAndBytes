#ifndef ZERO_COPY_SERVER_H
#define ZERO_COPY_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <liburing.h>
#include <stdbool.h>

#define QUEUE_DEPTH 256
#define BLOCK_SZ    4096
#define READ_SZ     4096  // Reduced buffer size for better handling
#define SERVER_PORT 8000
#define MAX_CONNECTIONS 1024

// Event types for better event handling
enum {
    EVENT_TYPE_ACCEPT = 0,
    EVENT_TYPE_READ,
    EVENT_TYPE_WRITE
};

struct conn_info {
    int fd;
    bool in_use;
    void *buf;
    size_t buf_size;
};

struct io_data {
    int event_type;
    int fd;
    struct iovec iov;
    struct conn_info *conn;
};

void fatal_error(const char *syscall);
void setup_io_uring(struct io_uring *ring);
int setup_listening_socket(int port);
void add_accept_request(struct io_uring *ring, int server_socket, struct conn_info *conn_table);
void add_read_request(struct io_uring *ring, struct conn_info *conn);
void handle_completion(struct io_uring *ring, struct io_uring_cqe *cqe, struct conn_info *conn_table);
struct conn_info *init_conn_table(void);
void cleanup_conn(struct conn_info *conn);

#endif