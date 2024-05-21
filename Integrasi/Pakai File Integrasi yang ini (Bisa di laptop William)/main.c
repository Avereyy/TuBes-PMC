#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "antColonyOptimization.h"
#include "DFS.h"
#include "Genetic_biar sesuai laptop william outputnya.h"
#include "BruteForce.h"
#include"PSO.h"
#include "branchBound.h"
#include "BFS.h"

int main(){
    char filename[255];
    char startingCity[255];
    printf("Nama file csv: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';
    printf("Kota Awal: ");
    fgets(startingCity, sizeof(startingCity), stdin);
    startingCity[strcspn(startingCity, "\n")] = '\0';

    antColonyOptimization(filename,startingCity);
    genetic(filename,startingCity);
    bruteForce_final(filename,startingCity);
    PSO(filename,startingCity);
    branch(filename,startingCity);
    BFS(filename,startingCity);
    DFS_final(filename);
}