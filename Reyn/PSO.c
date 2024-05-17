#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_CITIES 100
#define SWARM_SIZE 30
#define MAX_ITERATIONS 1000
#define W 0.5        // Inertia weight
#define C1 1.0       // Cognitive (particle's best)
#define C2 1.0       // Social (global best)

typedef struct {
    char name[50];
    double latitude;
    double longitude;
} City;

typedef struct {
    int path[MAX_CITIES];
    double fitness;
    int pbest[MAX_CITIES];
    double pbest_fitness;
    double velocity[MAX_CITIES];
} Particle;

double deg2rad(double deg) {
    return deg * (3.14159265359 / 180);
}

// Jarak antar dua kota
double hitungJarak(City kota1, City kota2) {
    double lat1 = deg2rad(kota1.latitude);
    double lon1 = deg2rad(kota1.longitude);
    double lat2 = deg2rad(kota2.latitude);
    double lon2 = deg2rad(kota2.longitude);
    
    double dlon = lon2 - lon1;
    double dlat = lat2 - lat1;
    
    double jarak = 2 * 6371 * asin(sqrt(pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2)));
    
    return jarak;
}

// Jarak semuanya
double calculateTotalDistance(City cities[], int path[], int numCities) {
    double totalDistance = 0.0;
    for (int i = 0; i < numCities - 1; i++) {
        totalDistance += hitungJarak(cities[path[i]], cities[path[i + 1]]);
    }
    totalDistance += hitungJarak(cities[path[numCities - 1]], cities[path[0]]); // return to start
    return totalDistance;
}

void initializeParticles(Particle particles[], int numCities, int swarmSize) {
    int tempPath[MAX_CITIES];
    for (int i = 0; i < numCities; i++) {
        tempPath[i] = i;
    }
    for (int i = 0; i < swarmSize; i++) {
        memcpy(particles[i].path, tempPath, sizeof(tempPath));
        for (int j = 0; j < numCities; j++) {
            int r = rand() % numCities;
            int temp = particles[i].path[j];
            particles[i].path[j] = particles[i].path[r];
            particles[i].path[r] = temp;
        }
        particles[i].fitness = INFINITY;
        particles[i].pbest_fitness = INFINITY;
        memset(particles[i].velocity, 0, sizeof(particles[i].velocity));
    }
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void updateVelocityAndPosition(Particle particles[], int numCities, int swarmSize, int gbest[], double inertia, double cognitive, double social) {
    for (int i = 0; i < swarmSize; i++) {
        for (int j = 0; j < numCities; j++) {
            double r1 = (double)rand() / RAND_MAX;
            double r2 = (double)rand() / RAND_MAX;
            particles[i].velocity[j] = inertia * particles[i].velocity[j] + cognitive * r1 * (particles[i].pbest[j] - particles[i].path[j]) + social * r2 * (gbest[j] - particles[i].path[j]);
            // Limit velocity to prevent large changes
            if (particles[i].velocity[j] > 0.5) particles[i].velocity[j] = 0.5;
            if (particles[i].velocity[j] < -0.5) particles[i].velocity[j] = -0.5;
        }
        for (int j = 0; j < numCities; j++) {
            int swapIndex = (j + (int)particles[i].velocity[j]) % numCities;
            if (swapIndex < 0) swapIndex += numCities;
            swap(&particles[i].path[j], &particles[i].path[swapIndex]);
        }
    }
}

void copyArray(int src[], int dest[], int size) {
    for (int i = 0; i < size; i++) {
        dest[i] = src[i];
    }
}

int main() {
    srand(time(NULL));

    char fileName[50];
    char kotaTujuan[50];

    printf("Enter list of kota file name: ");
    scanf("%s", fileName);

    printf("Enter starting point: ");
    scanf("%s", kotaTujuan);

    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("File tidak ditemukan atau File kosong.\n");
        return 1;
    }

    int numKota = 0;
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        numKota++;
    }
    rewind(file);

    City kota[numKota];
    char cityName[50];
    double latitude, longitude;
    int index = 0;
    while (fscanf(file, "%49[^,],%lf,%lf\n", cityName, &latitude, &longitude) == 3) {
        strcpy(kota[index].name, cityName);
        kota[index].latitude = latitude;
        kota[index].longitude = longitude;
        index++;
    }
    fclose(file);

    int kotaTujuanIndex = -1;
    for (int i = 0; i < numKota; i++) {
        if (strcmp(kota[i].name, kotaTujuan) == 0) {
            kotaTujuanIndex = i;
            break;
        }
    }
    if (kotaTujuanIndex == -1) {
        printf("Kota tujuan tidak ditemukan.\n");
        return 1;
    }

    Particle particles[SWARM_SIZE];
    int gbest[MAX_CITIES];
    double gbest_fitness = INFINITY;

// Particles

    initializeParticles(particles, numKota, SWARM_SIZE);

// PSO

    for (int iter = 0; iter < MAX_ITERATIONS; iter++) {
        for (int i = 0; i < SWARM_SIZE; i++) {
            particles[i].fitness = calculateTotalDistance(kota, particles[i].path, numKota);

            if (particles[i].fitness < particles[i].pbest_fitness) {
                particles[i].pbest_fitness = particles[i].fitness;
                copyArray(particles[i].path, particles[i].pbest, numKota);
            }

            if (particles[i].fitness < gbest_fitness) {
                gbest_fitness = particles[i].fitness;
                copyArray(particles[i].path, gbest, numKota);
            }
        }
        updateVelocityAndPosition(particles, numKota, SWARM_SIZE, gbest, W, C1, C2);
    }

    printf("Best route found:\n");
    for (int i = 0; i < numKota; i++) {
        printf("%s -> ", kota[gbest[i]].name);
    }
    printf("%s\n", kota[gbest[0]].name);

    printf("Best route distance: %.5f km\n", gbest_fitness);

    return 0;
}
