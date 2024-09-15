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
    printf("Size of GamePacket: %zu bytes\n", sizeof(struct GamePacket));
    return 0;
}
