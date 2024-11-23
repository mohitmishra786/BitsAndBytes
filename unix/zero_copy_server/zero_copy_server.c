#include "zero_copy_server.h"

void fatal_error(const char *syscall) {
    perror(syscall);
    exit(1);
}

struct conn_info *init_conn_table(void) {
    struct conn_info *conn_table = calloc(MAX_CONNECTIONS, sizeof(struct conn_info));
    if (!conn_table) {
        fatal_error("calloc");
    }
    return conn_table;
}

void cleanup_conn(struct conn_info *conn) {
    if (!conn) return;
    
    if (conn->fd >= 0) {
        close(conn->fd);
        conn->fd = -1;
    }
    
    if (conn->buf) {
        munmap(conn->buf, conn->buf_size);
        conn->buf = NULL;
    }
    
    conn->in_use = false;
    conn->buf_size = 0;
}

void setup_io_uring(struct io_uring *ring) {
    struct io_uring_params params;
    memset(&params, 0, sizeof(params));
    
    if (io_uring_queue_init_params(QUEUE_DEPTH, ring, &params) < 0) {
        fatal_error("io_uring_queue_init_params");
    }
}

int setup_listening_socket(int port) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        fatal_error("socket");
    }

    int enable = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        fatal_error("setsockopt");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        fatal_error("bind");
    }

    if (listen(server_socket, SOMAXCONN) < 0) {
        fatal_error("listen");
    }

    return server_socket;
}

void add_accept_request(struct io_uring *ring, int server_socket, struct conn_info *conn_table) {
    struct io_data *data = malloc(sizeof(*data));
    if (!data) {
        fatal_error("malloc");
    }

    data->event_type = EVENT_TYPE_ACCEPT;
    data->fd = server_socket;
    data->conn = conn_table;

    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    if (!sqe) {
        free(data);
        fatal_error("io_uring_get_sqe");
    }

    io_uring_prep_accept(sqe, server_socket, NULL, NULL, 0);
    io_uring_sqe_set_data(sqe, data);
}

void add_read_request(struct io_uring *ring, struct conn_info *conn) {
    struct io_data *data = malloc(sizeof(*data));
    if (!data) {
        fatal_error("malloc");
    }

    data->event_type = EVENT_TYPE_READ;
    data->fd = conn->fd;
    data->conn = conn;

    // Allocate or reuse buffer
    if (!conn->buf) {
        conn->buf = mmap(NULL, READ_SZ, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
        if (conn->buf == MAP_FAILED) {
            free(data);
            fatal_error("mmap");
        }
        conn->buf_size = READ_SZ;
    }

    data->iov.iov_base = conn->buf;
    data->iov.iov_len = conn->buf_size;

    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    if (!sqe) {
        free(data);
        fatal_error("io_uring_get_sqe");
    }

    io_uring_prep_readv(sqe, conn->fd, &data->iov, 1, 0);
    io_uring_sqe_set_data(sqe, data);
}

void handle_completion(struct io_uring *ring, struct io_uring_cqe *cqe, struct conn_info *conn_table) {
    struct io_data *data = (struct io_data *)io_uring_cqe_get_data(cqe);
    if (!data) {
        fprintf(stderr, "No data found in completion queue\n");
        return;
    }

    switch (data->event_type) {
        case EVENT_TYPE_ACCEPT: {
            int client_fd = cqe->res;
            if (client_fd < 0) {
                fprintf(stderr, "Accept failed: %s\n", strerror(-client_fd));
            } else {
                printf("Accepted new connection: %d\n", client_fd);
                
                struct conn_info *conn = NULL;
                for (int i = 0; i < MAX_CONNECTIONS; i++) {
                    if (!conn_table[i].in_use) {
                        conn = &conn_table[i];
                        break;
                    }
                }

                if (conn) {
                    conn->fd = client_fd;
                    conn->in_use = true;
                    add_read_request(ring, conn);
                } else {
                    fprintf(stderr, "No free connection slots\n");
                    close(client_fd);
                }
            }
            add_accept_request(ring, data->fd, conn_table);
            break;
        }
        
        case EVENT_TYPE_READ: {
            if (cqe->res < 0) {
                fprintf(stderr, "Read failed: %s\n", strerror(-cqe->res));
                cleanup_conn(data->conn);
            } else if (cqe->res == 0) {
                printf("Connection closed\n");
                cleanup_conn(data->conn);
            } else {
                printf("Read %d bytes from client\n", cqe->res);
                write(data->fd, data->conn->buf, cqe->res);
                add_read_request(ring, data->conn);
            }
            break;
        }
    }

    free(data);
}