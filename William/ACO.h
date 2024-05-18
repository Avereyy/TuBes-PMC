#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <time.h>
#include "haversine.h"

#define MAX_CITIES 100
#define MAX_ANTS 1000
#define ALPHA 1.0
#define BETA 5.0
#define EVAPORATION 0.5
#define Q 100
#define ITERATIONS 1000

typedef struct {
    int path[MAX_CITIES];
    double length;
} Ant;


int readCities(const char* filename, City cities[]) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file!\n");
        return 0;
    }

    int count = 0;
    while (fscanf(file, "%[^,],%lf,%lf\n", cities[count].name, &cities[count].latitude, &cities[count].longitude) == 3) {
        count++;
    }

    fclose(file);
    return count;
}

void initializePheromones(double pheromones[MAX_CITIES][MAX_CITIES], int numCities) {
    for (int i = 0; i < numCities; i++) {
        for (int j = 0; j < numCities; j++) {
            pheromones[i][j] = 1.0;
        }
    }
}

void initializeAnts(Ant ants[], int numAnts, int numCities, int startCity) {
    for (int i = 0; i < numAnts; i++) {
        for (int j = 0; j < numCities; j++) {
            ants[i].path[j] = -1;
        }
        ants[i].path[0] = startCity;
        ants[i].length = 0.0;
    }
}

double calculateProbability(int from, int to, double pheromones[MAX_CITIES][MAX_CITIES], double distances[MAX_CITIES][MAX_CITIES], int visited[], int numCities) {
    if (visited[to]) {
        return 0.0;
    }

    double pheromone = pow(pheromones[from][to], ALPHA);
    double heuristic = pow(1.0 / distances[from][to], BETA);

    return pheromone * heuristic;
}

int chooseNextCity(int currentCity, double pheromones[MAX_CITIES][MAX_CITIES], double distances[MAX_CITIES][MAX_CITIES], int visited[], int numCities) {
    double probabilities[MAX_CITIES] = {0};
    double sum = 0.0;

    for (int i = 0; i < numCities; i++) {
        if (!visited[i]) {
            probabilities[i] = calculateProbability(currentCity, i, pheromones, distances, visited, numCities);
            sum += probabilities[i];
        }
    }

    if (sum == 0.0) {
        return -1;
    }

    double random = ((double) rand() / RAND_MAX) * sum;

    for (int i = 0; i < numCities; i++) {
        if (!visited[i]) {
            random -= probabilities[i];
            if (random <= 0) {
                return i;
            }
        }
    }

    return -1;
}

void updatePheromones(double pheromones[MAX_CITIES][MAX_CITIES], Ant ants[], int numAnts, int numCities) {
    double deltaPheromones[MAX_CITIES][MAX_CITIES] = {0};

    for (int k = 0; k < numAnts; k++) {
        for (int i = 0; i < numCities - 1; i++) {
            int from = ants[k].path[i];
            int to = ants[k].path[i + 1];
            deltaPheromones[from][to] += Q / ants[k].length;
            deltaPheromones[to][from] += Q / ants[k].length;
        }
        int from = ants[k].path[numCities - 1];
        int to = ants[k].path[0];
        deltaPheromones[from][to] += Q / ants[k].length;
        deltaPheromones[to][from] += Q / ants[k].length;
    }

    for (int i = 0; i < numCities; i++) {
        for (int j = 0; j < numCities; j++) {
            pheromones[i][j] *= (1.0 - EVAPORATION);
            pheromones[i][j] += deltaPheromones[i][j];
            if (pheromones[i][j] < 0.01) {
                pheromones[i][j] = 0.01;
            } else if (pheromones[i][j] > 100.0) {
                pheromones[i][j] = 100.0;
            }
        }
    }
}

double calculatePathLength(Ant* ant, double distances[MAX_CITIES][MAX_CITIES], int numCities) {
    double length = 0.0;
    for (int i = 0; i < numCities - 1; i++) {
        length += distances[ant->path[i]][ant->path[i + 1]];
    }
    length += distances[ant->path[numCities - 1]][ant->path[0]];
    return length;
}

void twoOptSwap(int* path, int i, int k, int numCities) {
    while (i < k) {
        int temp = path[i];
        path[i] = path[k];
        path[k] = temp;
        i++;
        k--;
    }
}

void localSearch2Opt(Ant* ant, double distances[MAX_CITIES][MAX_CITIES], int numCities) {
    int improve = 0;
    double bestDistance = ant->length;
    do {
        improve = 0;
        for (int i = 1; i < numCities - 1; i++) {
            for (int k = i + 1; k < numCities; k++) {
                twoOptSwap(ant->path, i, k, numCities);
                double newDistance = calculatePathLength(ant, distances, numCities);
                if (newDistance < bestDistance) {
                    bestDistance = newDistance;
                    ant->length = newDistance;
                    improve = 1;
                } else {
                    twoOptSwap(ant->path, i, k, numCities); 
                }
            }
        }
    } while (improve);
}

void acoTSP(City cities[], int numCities, int numAnts, int startCity, Ant* bestAnt) {
    double distances[MAX_CITIES][MAX_CITIES];
    double pheromones[MAX_CITIES][MAX_CITIES];

    for (int i = 0; i < numCities; i++) {
        for (int j = 0; j < numCities; j++) {
            if (i != j) {
                distances[i][j] = haversineDistance(cities[i], cities[j]);
            } else {
                distances[i][j] = DBL_MAX;
            }
            pheromones[i][j] = 1.0;
        }
    }

    bestAnt->length = DBL_MAX;

    for (int iteration = 0; iteration < ITERATIONS; iteration++) {
        Ant ants[MAX_ANTS];
        initializeAnts(ants, numAnts, numCities, startCity);

        for (int k = 0; k < numAnts; k++) {
            int visited[MAX_CITIES] = {0};
            visited[startCity] = 1;

            for (int i = 1; i < numCities; i++) {
                int currentCity = ants[k].path[i - 1];
                int nextCity = chooseNextCity(currentCity, pheromones, distances, visited, numCities);
                if (nextCity == -1) break;
                ants[k].path[i] = nextCity;
                visited[nextCity] = 1;
            }

            ants[k].length = calculatePathLength(&ants[k], distances, numCities);
            localSearch2Opt(&ants[k], distances, numCities); 

            if (ants[k].length < bestAnt->length) {
                *bestAnt = ants[k];
            }
        }

        updatePheromones(pheromones, ants, numAnts, numCities);
    }
}
