#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

typedef struct {
    float x, y, z;
} Vector3;

typedef struct {
    Vector3 origin;
    Vector3 direction;
} Ray;

typedef struct {
    Vector3 v0, v1, v2;
} Triangle;

Vector3 subtract(Vector3 a, Vector3 b) {
    Vector3 result = {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
    return result;
}

Vector3 cross(Vector3 a, Vector3 b) {
    Vector3 result = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
    return result;
}

float dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Möller–Trumbore intersection algorithm
int ray_triangle_intersection(Ray ray, Triangle triangle, float* t) {
    const float EPSILON = 0.0000001;
    Vector3 edge1 = subtract(triangle.v1, triangle.v0);
    Vector3 edge2 = subtract(triangle.v2, triangle.v0);
    Vector3 h = cross(ray.direction, edge2);
    float a = dot(edge1, h);
    
    if (a > -EPSILON && a < EPSILON)
        return 0;  // Ray is parallel to triangle
        
    float f = 1.0 / a;
    Vector3 s = subtract(ray.origin, triangle.v0);
    float u = f * dot(s, h);
    
    if (u < 0.0 || u > 1.0)
        return 0;
        
    Vector3 q = cross(s, edge1);
    float v = f * dot(ray.direction, q);
    
    if (v < 0.0 || u + v > 1.0)
        return 0;
        
    *t = f * dot(edge2, q);
    return *t > EPSILON;
}

int main() {
    const int NUM_RAYS = 1000000;
    const int NUM_TRIANGLES = 1000;
    
    // Create sample rays and triangles
    Ray* rays = (Ray*)malloc(NUM_RAYS * sizeof(Ray));
    Triangle* triangles = (Triangle*)malloc(NUM_TRIANGLES * sizeof(Triangle));
    
    // Initialize random rays and triangles
    srand(time(NULL));
    for (int i = 0; i < NUM_RAYS; i++) {
        rays[i].origin = (Vector3){
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX
        };
        rays[i].direction = (Vector3){
            (float)rand() / RAND_MAX - 0.5f,
            (float)rand() / RAND_MAX - 0.5f,
            (float)rand() / RAND_MAX - 0.5f
        };
    }
    
    for (int i = 0; i < NUM_TRIANGLES; i++) {
        triangles[i].v0 = (Vector3){
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX
        };
        triangles[i].v1 = (Vector3){
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX
        };
        triangles[i].v2 = (Vector3){
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX
        };
    }
    
    // Perform intersection tests
    int total_intersections = 0;
    double start_time = omp_get_wtime();
    
    #pragma omp parallel for reduction(+:total_intersections)
    for (int i = 0; i < NUM_RAYS; i++) {
        for (int j = 0; j < NUM_TRIANGLES; j++) {
            float t;
            if (ray_triangle_intersection(rays[i], triangles[j], &t)) {
                total_intersections++;
            }
        }
    }
    
    double end_time = omp_get_wtime();
    
    printf("Processed %d ray-triangle intersections\n", 
           NUM_RAYS * NUM_TRIANGLES);
    printf("Found %d intersections\n", total_intersections);
    printf("Time taken: %.3f seconds\n", end_time - start_time);
    
    free(rays);
    free(triangles);
    return 0;
}