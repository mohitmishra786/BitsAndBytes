#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4433

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = inet_addr("127.0.0.1")
    };
    
    char *message = "QUIC-like message";
    sendto(sockfd, message, 17, 0,
          (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    
    char buffer[1024];
    recvfrom(sockfd, buffer, 1024, 0, NULL, NULL);
    printf("Received: %s\n", buffer);
    return 0;
}
