#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define PAGE_SIZE 4096
#define MAX_RECORDS 100

typedef struct {
    int id;
    char name[50];
} Record;

typedef struct {
    int record_count;
    Record records[MAX_RECORDS];
} Database;

void* map_file(const char* filename, size_t* size) {
    int fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Error opening file");
        return NULL;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("Error getting file size");
        close(fd);
        return NULL;
    }

    *size = sb.st_size;

    // If the file is empty, initialize it with the Database structure
    if (*size == 0) {
        *size = sizeof(Database);
        if (ftruncate(fd, *size) == -1) {
            perror("Error setting file size");
            close(fd);
            return NULL;
        }
    }

    void* addr = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return NULL;
    }

    close(fd);
    return addr;
}

int insert_record(Database* db, int id, const char* name) {
    if (db->record_count >= MAX_RECORDS) {
        printf("Database is full\n");
        return 0;
    }

    Record* record = &db->records[db->record_count];
    record->id = id;
    strncpy(record->name, name, sizeof(record->name) - 1);
    record->name[sizeof(record->name) - 1] = '\0';

    db->record_count++;
    return 1;
}

Record* find_record(Database* db, int id) {
    for (int i = 0; i < db->record_count; i++) {
        if (db->records[i].id == id) {
            return &db->records[i];
        }
    }
    return NULL;
}

int main() {
    size_t size;
    Database* db = (Database*)map_file("database.dat", &size);

    if (!db) {
        return 1;
    }

    // Insert some records
    insert_record(db, 1, "Alice");
    insert_record(db, 2, "Bob");
    insert_record(db, 3, "Charlie");

    // Find and print a record
    Record* record = find_record(db, 2);
    if (record) {
        printf("Found record: ID=%d, Name=%s\n", record->id, record->name);
    } else {
        printf("Record not found\n");
    }

    // Print all records
    printf("All records:\n");
    for (int i = 0; i < db->record_count; i++) {
        printf("ID=%d, Name=%s\n", db->records[i].id, db->records[i].name);
    }

    // Unmap the file
    if (munmap(db, size) == -1) {
        perror("Error unmapping file");
    }

    return 0;
}
