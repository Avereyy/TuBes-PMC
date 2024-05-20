#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <time.h>
#include "ACO.h"

int main() {
    char filename[100] ;
    char startingCity[50] ;

    printf("Nama file csv: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';
    City cities[MAX_CITIES];
    int numCities = readCities(filename, cities);
    if (numCities == 0) {
        printf("File Kosong\n");
        return 1;
    }
    printf("Kota Awal: ");
    fgets(startingCity, sizeof(startingCity), stdin);
    startingCity[strcspn(startingCity, "\n")] = '\0';

    int startCityIndex = -1;
    for (int i = 0; i < numCities; i++) {
        if (strcmp(cities[i].name, startingCity) == 0) {
            startCityIndex = i;
            break;
        }
    }

    if (startCityIndex == -1) {
        printf("Kota tidak ditemukan\n");
        return 1;
    }

    srand(time(NULL));

    Ant bestAnt;
    clock_t start = clock();
    acoTSP(cities, numCities, numCities, startCityIndex, &bestAnt);
    clock_t end = clock();
    double timeElapsed = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Path Terpendek:\n");
    for (int i = 0; i < numCities; i++) {
        if (strlen(cities[bestAnt.path[i]].name)==0){
            acoTSP(cities, numCities, numCities, startCityIndex, &bestAnt);
            i=0;
        }

    }
    for (int i = 0; i < numCities; i++) {
        printf("%s -> ", cities[bestAnt.path[i]].name);
    }
    printf("%s\n", cities[bestAnt.path[0]].name);
    printf("Jarak Tempuh: %f km\n", bestAnt.length);
    printf("Waktu yang diperlukan program: %f seconds\n", timeElapsed);

    return 0;
}
