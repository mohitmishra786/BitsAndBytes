#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 4433

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };
    
    bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    
    char buffer[1024];
    struct sockaddr_in cli_addr;
    socklen_t len = sizeof(cli_addr);
    
    while(1) {
        int n = recvfrom(sockfd, buffer, 1024, 0, 
                        (struct sockaddr*)&cli_addr, &len);
        sendto(sockfd, buffer, n, 0, 
              (struct sockaddr*)&cli_addr, len);
    }
}
