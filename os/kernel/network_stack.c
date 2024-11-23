#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>

struct application_layer {
    int socket_fd;
    char buffer[1024];
    size_t buf_len;
};

struct transport_layer {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq_num;
    uint32_t ack_num;
    uint16_t checksum;
};

struct network_layer {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint8_t protocol;
    uint8_t ttl;
};

struct link_layer {
    unsigned char src_mac[6];
    unsigned char dst_mac[6];
    uint16_t ethertype;
};

void process_application_layer(struct application_layer *app, const char *data) {
    printf("\n=== Application Layer Processing ===\n");
    strncpy(app->buffer, data, sizeof(app->buffer) - 1);
    app->buf_len = strlen(data);
    printf("Data received from application: %s\n", app->buffer);
}

void process_transport_layer(struct transport_layer *transport, struct application_layer *app) {
    printf("\n=== Transport Layer Processing ===\n");
    transport->src_port = 12345;
    transport->dst_port = 80;
    transport->seq_num = 1000;
    transport->ack_num = 0;
    
    // simple checksum
    transport->checksum = 0;
    for (size_t i = 0; i < app->buf_len; i++) {
        transport->checksum += app->buffer[i];
    }
    
    printf("Source Port: %d\n", transport->src_port);
    printf("Destination Port: %d\n", transport->dst_port);
    printf("Checksum: 0x%04x\n", transport->checksum);
}

void process_network_layer(struct network_layer *network) {
    printf("\n=== Network Layer Processing ===\n");
    network->src_ip = inet_addr("192.168.1.100");
    network->dst_ip = inet_addr("192.168.1.1");
    network->protocol = 6; // TCP
    network->ttl = 64;
    
    char src_ip_str[INET_ADDRSTRLEN];
    char dst_ip_str[INET_ADDRSTRLEN];
    
    inet_ntop(AF_INET, &network->src_ip, src_ip_str, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &network->dst_ip, dst_ip_str, INET_ADDRSTRLEN);
    
    printf("Source IP: %s\n", src_ip_str);
    printf("Destination IP: %s\n", dst_ip_str);
    printf("TTL: %d\n", network->ttl);
}

void process_link_layer(struct link_layer *link) {
    printf("\n=== Link Layer Processing ===\n");
    unsigned char src_mac[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
    unsigned char dst_mac[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    
    memcpy(link->src_mac, src_mac, 6);
    memcpy(link->dst_mac, dst_mac, 6);
    link->ethertype = ETH_P_IP;
    
    printf("Source MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
           link->src_mac[0], link->src_mac[1], link->src_mac[2],
           link->src_mac[3], link->src_mac[4], link->src_mac[5]);
    printf("Destination MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
           link->dst_mac[0], link->dst_mac[1], link->dst_mac[2],
           link->dst_mac[3], link->dst_mac[4], link->dst_mac[5]);
}

int main() {
    printf("Starting Network Stack Simulation...\n");
    
    struct application_layer app = {0};
    struct transport_layer transport = {0};
    struct network_layer network = {0};
    struct link_layer link = {0};
    
    const char *user_data = "Hello, Network Stack!";
    
    process_application_layer(&app, user_data);
    process_transport_layer(&transport, &app);
    process_network_layer(&network);
    process_link_layer(&link);
    
    printf("\n=== Packet Processing Complete ===\n");
    printf("Data successfully processed through all network layers\n");
    
    return 0;
}