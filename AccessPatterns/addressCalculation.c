#include <stdio.h>
#include <stdlib.h>

struct Example {
    char c;
    int i;
    short s;
};

void printArrayAddresses(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("Address of arr[%d]: %p\n", i, (void *)&arr[i]);
    }
}

void print2DArrayAddresses(int rows, int cols, int (*arr)[cols]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("Address of arr[%d][%d]: %p\n", i, j, (void *)&arr[i][j]);
        }
    }
}

int main() {
    // 1D Array
    int staticArray[5] = {10, 20, 30, 40, 50};
    printf("1D Static Array:\n");
    printArrayAddresses(staticArray, 5);
    
    // Dynamic 1D Array
    int *dynamicArray = (int *)malloc(5 * sizeof(int));
    for (int i = 0; i < 5; i++) {
        dynamicArray[i] = (i + 1) * 10;
    }
    printf("\nDynamic 1D Array:\n");
    printArrayAddresses(dynamicArray, 5);
    
    // 2D Static Array
    int staticMatrix[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    printf("\n2D Static Array:\n");
    print2DArrayAddresses(3, 3, staticMatrix);
    
    // Dynamic 2D Array (contiguous allocation)
    int rows = 3, cols = 3;
    int (*dynamicMatrix)[cols] = malloc(rows * cols * sizeof(int));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            dynamicMatrix[i][j] = i * cols + j + 1;
        }
    }
    printf("\nDynamic 2D Array (contiguous):\n");
    print2DArrayAddresses(rows, cols, dynamicMatrix);
    
    // Dynamic 2D Array (array of pointers)
    int **dynamicMatrix2 = malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {
        dynamicMatrix2[i] = malloc(cols * sizeof(int));
        for (int j = 0; j < cols; j++) {
            dynamicMatrix2[i][j] = i * cols + j + 1;
        }
    }
    printf("\nDynamic 2D Array (array of pointers):\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("Address of dynamicMatrix2[%d][%d]: %p\n", i, j, (void *)&dynamicMatrix2[i][j]);
        }
    }
    
    // Structure alignment and padding
    struct Example ex;
    printf("\nStructure Alignment and Padding:\n");
    printf("Address of ex.c: %p\n", (void *)&ex.c);
    printf("Address of ex.i: %p\n", (void *)&ex.i);
    printf("Address of ex.s: %p\n", (void *)&ex.s);
    printf("Size of struct Example: %zu\n", sizeof(struct Example));
    
    // Clean up
    free(dynamicArray);
    free(dynamicMatrix);
    for (int i = 0; i < rows; i++) {
        free(dynamicMatrix2[i]);
    }
    free(dynamicMatrix2);
    
    return 0;
}
