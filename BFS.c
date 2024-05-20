#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define PI 3.14159265358979323846
#define RADIUS 6371 // Radius of the Earth in kilometers

// Structure to represent a city
struct City {
    char name[50];
    double latitude;
    double longitude;
};

// Queue structure for BFS
struct Queue {
    int front, rear, size;
    unsigned capacity;
    int* array;
};

// Function to create a queue
struct Queue* createQueue(unsigned capacity) {
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;  
    queue->array = (int*) malloc(queue->capacity * sizeof(int));
    return queue;
}

// Queue is full when size becomes equal to the capacity
int isFull(struct Queue* queue) {
    return (queue->size == queue->capacity);
}

// Queue is empty when size is 0
int isEmpty(struct Queue* queue) {
    return (queue->size == 0);
}

// Function to add an item to the queue
void enqueue(struct Queue* queue, int item) {
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}

// Function to remove an item from queue
int dequeue(struct Queue* queue) {
    if (isEmpty(queue))
        return -1;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to calculate distance between two cities using Haversine formula
double haversine(struct City city1, struct City city2) {
    double Lati1 = city1.latitude;
    double Lati2 = city2.latitude;
    double Long1 = city1.longitude;
    double Long2 = city2.longitude;

    double R = 6371; // Earth's radius in kilometers
    double DLati = (Lati2 - Lati1) * PI / 180; // Convert to radians
    double DLong = (Long2 - Long1) * PI / 180;

    double distance = 2 * R * asin(sqrt(pow(sin(DLati / 2), 2) +
                                        cos(Lati1 * PI / 180) * cos(Lati2 * PI / 180) *
                                        pow(sin(DLong / 2), 2)));
    return distance;
}

// Function to compute distance matrix
void computeDistanceMatrix(struct City *cities, int numCities, double **distanceMatrix) {
    for (int i = 0; i < numCities; i++) {
        for (int j = 0; j < numCities; j++) {
            if (i == j) {
                distanceMatrix[i][j] = 0;
            } else {
                distanceMatrix[i][j] = haversine(cities[i], cities[j]);
            }
        }
    }
}

// Function to find shortest route using Breadth-First Search (BFS)
void findShortestRoute(struct City *cities, int numCities, double **distanceMatrix, int *route, int *shortestRoute, int startCityIndex, double *totalDistance) {
    bool visited[numCities];
    memset(visited, false, sizeof(visited)); // Initialize all cities as unvisited
    struct Queue* queue = createQueue(numCities);
    
    // Start from the specified starting city
    visited[startCityIndex] = true;
    enqueue(queue, startCityIndex);

    int routeIndex = 0;
    *totalDistance = 0.0;

    while (!isEmpty(queue)) {
        int currentCityIndex = dequeue(queue);
        route[routeIndex++] = currentCityIndex; // Record the path

        for (int i = 0; i < numCities; i++) {
            if (!visited[i]) {
                visited[i] = true;
                enqueue(queue, i);
                *totalDistance += distanceMatrix[currentCityIndex][i];
            }
        }
    }

    // Add distance back to the starting city
    *totalDistance += distanceMatrix[route[numCities - 1]][startCityIndex];

    // Copy the route found
    memcpy(shortestRoute, route, numCities * sizeof(int));
}

// Function to print route
void printRoute(struct City *cities, int numCities, int *route, double totalDistance) {
    printf("Best route found:\n");
    printf("Route: ");
    for (int i = 0; i < numCities; i++) {
        printf("%s -> ", cities[route[i]].name);
    }
    printf("%s\n", cities[route[0]].name); // Back to starting city
    printf("Best route distance: %.5f km\n", totalDistance);
}

int main() {
    // List of cities
    struct City cities[] = {
        {"Bandung", -6.9175, 107.6191},
        {"Bali", -8.3405, 115.0920},
        {"Semarang", -7.0051, 110.4381},
        {"Surabaya", -7.2575, 112.7521}
    };
    int numCities = sizeof(cities) / sizeof(cities[0]);

    // Allocate memory for the distance matrix
    double **distanceMatrix = (double **)malloc(numCities * sizeof(double *));
    for (int i = 0; i < numCities; i++) {
        distanceMatrix[i] = (double *)malloc(numCities * sizeof(double));
    }

    // Compute the distance matrix
    computeDistanceMatrix(cities, numCities, distanceMatrix);

    // Input starting point
    char startCityName[50];
    int startCityIndex = -1;
    while (startCityIndex == -1) {
        printf("Enter starting point: ");
        scanf("%s", startCityName);
        for (int i = 0; i < numCities; i++) {
            if (strcmp(cities[i].name, startCityName) == 0) {
                startCityIndex = i;
                break;
            }
        }
        if (startCityIndex == -1) {
            printf("Error: Starting city not found. Please enter a valid city name.\n");
        }
    }

    // Initialize variables
    int *route = (int*)malloc(numCities * sizeof(int));
    int *shortestRoute = (int*)malloc(numCities * sizeof(int));
    double totalDistance = 0.0;

    // Find shortest route
    clock_t begin = clock();
    findShortestRoute(cities, numCities, distanceMatrix, route, shortestRoute, startCityIndex, &totalDistance);
    clock_t end = clock();

    // Print result
    printRoute(cities, numCities, shortestRoute, totalDistance);
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time elapsed: %.10f s\n", time_spent);

    // Free memory
    for (int i = 0; i < numCities; i++) {
        free(distanceMatrix[i]);
    }
    free(distanceMatrix);
    free(route);
    free(shortestRoute);

    return 0;
}
