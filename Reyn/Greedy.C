#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

typedef struct {
    char name[50];
    double latitude;
    double longitude;
} City;

double deg2rad(double deg) {
    return deg * (3.14159265359/180);
}
double hitungJarak(City kota1, City kota2) {
    double lat1 = deg2rad(kota1.latitude);
    double lon1 = deg2rad(kota1.longitude);
    double lat2 = deg2rad(kota2.latitude);
    double lon2 = deg2rad(kota2.longitude);
    
    double dlon = lon2 - lon1;
    double dlat = lat2 - lat1;
    
    double jarak = 2*6371*asin(sqrt(pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2)));
    
    return jarak;
}

int Index(City kota[], int numKota, char *cityName) {
    int i;
    for (i = 0; i < numKota; i++) {
        if (strcmp(kota[i].name, cityName) == 0) {
            return i;
        }
    }
    return -1;
}

double Djikstra(City kota[], int numKota, char *kotaTujuan, char *bestRoute[]) {
    int i, j;
    int visited[numKota];
    double Jarak = 0;
    int current = Index(kota, numKota, kotaTujuan);
    int next;
    
    memset(visited, 0, sizeof(visited));
    
    visited[current] = 1;
    bestRoute[0] = strdup(kotaTujuan);
    
    for (i = 0; i < numKota - 1; i++) {
        double tempJarak = -1;
        for (j = 0; j < numKota; j++) {
            if (!visited[j]) {
                double distance = hitungJarak(kota[current], kota[j]);
                if (tempJarak == -1 || distance < tempJarak) {
                    tempJarak = distance;
                    next = j;
                }
            }
        }
        visited[next] = 1;
        Jarak += tempJarak;
        current = next;
        bestRoute[i+1] = strdup(kota[current].name);
    }
    Jarak += hitungJarak(kota[current], kota[Index(kota, numKota, kotaTujuan)]);
    
    return Jarak;
}

int main() {
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
    
    char *bestRoute[numKota];
    clock_t start = clock();
    double Jarak = Djikstra(kota, numKota, kotaTujuan, bestRoute);
    clock_t end = clock();
    double timeElapsed = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Best route found:\n");
    for (int i = 0; i < numKota; i++) {
        printf("%s -> ", bestRoute[i]);
    }
    printf("%s\n", kotaTujuan);
    
    printf("Best route distance: %.5f km\n", Jarak);
    printf("Time elapsed: %.10f s\n", timeElapsed);
    
    for (int i = 0; i < numKota; i++) {
        free(bestRoute[i]);
    }
    
    return 0;
}
