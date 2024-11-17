#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    struct stat stdout_stat;
    struct stat file_stat;
    
    // Get information about stdout
    if (fstat(STDOUT_FILENO, &stdout_stat) == -1) {
        perror("fstat stdout failed");
        return 1;
    }
    
    // Create and get information about a regular file
    int fd = open("test.txt", O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror("open failed");
        return 1;
    }
    
    if (fstat(fd, &file_stat) == -1) {
        perror("fstat file failed");
        close(fd);
        return 1;
    }
    
    fprintf(stderr, "stdout block size: %ld\n", stdout_stat.st_blksize);
    fprintf(stderr, "file block size: %ld\n", file_stat.st_blksize);
    
    close(fd);
    return 0;
}
