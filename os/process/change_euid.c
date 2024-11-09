#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main() {
    uid_t ruid, euid, suid;

    if (getresuid(&ruid, &euid, &suid) == -1) {
        fprintf(stderr, "Error getting user IDs: %s\n", strerror(errno));
        return 1;
    }

    printf("Initial UIDs - Real: %d, Effective: %d, Saved: %d\n", ruid, euid, suid);

    // Attempt to change effective UID to 1000
    if (seteuid(1000) == -1) {
        fprintf(stderr, "Error changing effective UID: %s\n", strerror(errno));
        return 1;
    }

    if (getresuid(&ruid, &euid, &suid) == -1) {
        fprintf(stderr, "Error getting updated user IDs: %s\n", strerror(errno));
        return 1;
    }

    printf("Updated UIDs - Real: %d, Effective: %d, Saved: %d\n", ruid, euid, suid);

    return 0;
}
