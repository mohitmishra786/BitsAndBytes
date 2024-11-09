#include <stdio.h>
#include <stdint.h>

struct GamePacket {
    uint8_t packetType;
    uint32_t playerId;
    float positionX;
    float positionY;
    uint8_t health;
    uint16_t score;
};

int main() {
    struct GamePacket packet;
    
    printf("Address of packetType: %p\n", (void*)&packet.packetType);
    printf("Address of playerId:   %p\n", (void*)&packet.playerId);
    printf("Address of positionX:  %p\n", (void*)&packet.positionX);
    printf("Address of positionY:  %p\n", (void*)&packet.positionY);
    printf("Address of health:     %p\n", (void*)&packet.health);
    printf("Address of score:      %p\n", (void*)&packet.score);
    
    printf("Size of GamePacket: %zu bytes\n", sizeof(struct GamePacket));
    return 0;
}
