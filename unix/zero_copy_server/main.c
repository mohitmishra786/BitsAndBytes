#include "zero_copy_server.h"

int main() {
    struct io_uring ring;
    struct io_uring_cqe *cqe;
    struct conn_info *conn_table;
    int server_socket;

    // Initialize connection table
    conn_table = init_conn_table();

    // Initialize io_uring
    setup_io_uring(&ring);

    // Setup listening socket
    server_socket = setup_listening_socket(SERVER_PORT);
    printf("Server listening on port %d\n", SERVER_PORT);

    // Add initial accept request
    add_accept_request(&ring, server_socket, conn_table);

    // Event loop
    while (1) {
        int ret = io_uring_submit_and_wait(&ring, 1);
        if (ret < 0) {
            fatal_error("io_uring_submit_and_wait");
        }

        unsigned head;
        unsigned completed = 0;
        io_uring_for_each_cqe(&ring, head, cqe) {
            handle_completion(&ring, cqe, conn_table);
            completed++;
        }

        io_uring_cq_advance(&ring, completed);
    }

    // Cleanup
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        cleanup_conn(&conn_table[i]);
    }
    free(conn_table);
    io_uring_queue_exit(&ring);
    close(server_socket);
    return 0;
}