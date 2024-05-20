#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <stdbool.h>
#include <time.h>

#define M_PI 3.14159265358979323846
#define EARTH_RADIUS 6371

struct City {
    char Name[50];
    float Latitude;
    float Longitude;
    struct City* next;
};

void addCity(struct City** head, char* name, float latitude, float longitude) {
    struct City* newCity = (struct City*)malloc(sizeof(struct City));
    strcpy(newCity->Name, name);
    newCity->Latitude = latitude;
    newCity->Longitude = longitude;
    newCity->next = *head;
    *head = newCity;
}

void freeCityList(struct City* head) {
    struct City* tmp;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

float calculateDistance(float lat1, float lon1, float lat2, float lon2) {
    float dlat = (lat2 - lat1) * M_PI / 180.0;
    float dlon = (lon2 - lon1) * M_PI / 180.0;
    float a = sin(dlat / 2) * sin(dlat / 2) + cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) * sin(dlon / 2) * sin(dlon / 2);
    float c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS * c;
}

void displayPath(struct City* head, int path[], int n) {
    struct City* cities[n];
    struct City* current = head;
    for (int i = 0; i < n; i++) {
        cities[i] = current;
        current = current->next;
    }
    for (int i = 0; i < n; i++) {
        printf("%s -> ", cities[path[i]]->Name);
    }
    printf("%s\n", cities[path[0]]->Name);
}

void bfsTSP(struct City *head, int path[], int n, int startIdx) {
    float minDist = FLT_MAX;
    int minPath[n];

    struct City* cities[n];
    struct City* current = head;
    for (int i = 0; i < n; i++) {
        cities[i] = current;
        current = current->next;
    }

    struct QueueNode {
        int path[n];
        int level;
        float cost;
        struct QueueNode* next;
    };

    struct QueueNode* front = NULL;
    struct QueueNode* rear = NULL;

    struct QueueNode* startNode = (struct QueueNode*)malloc(sizeof(struct QueueNode));
    for (int i = 0; i < n; i++) {
        startNode->path[i] = -1;
    }
    startNode->path[0] = startIdx;
    startNode->level = 1;
    startNode->cost = 0;
    startNode->next = NULL;

    front = rear = startNode;

    while (front != NULL) {
        struct QueueNode* node = front;
        front = front->next;

        if (node->level == n) {
            node->cost += calculateDistance(cities[node->path[n-1]]->Latitude, cities[node->path[n-1]]->Longitude, cities[node->path[0]]->Latitude, cities[node->path[0]]->Longitude);
            if (node->cost < minDist) {
                minDist = node->cost;
                memcpy(minPath, node->path, n * sizeof(int));
            }
        } else {
            for (int i = 0; i < n; i++) {
                bool found = false;
                for (int j = 0; j < node->level; j++) {
                    if (node->path[j] == i) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    struct QueueNode* newNode = (struct QueueNode*)malloc(sizeof(struct QueueNode));
                    memcpy(newNode->path, node->path, n * sizeof(int));
                    newNode->path[node->level] = i;
                    newNode->level = node->level + 1;
                    newNode->cost = node->cost + calculateDistance(cities[node->path[node->level-1]]->Latitude, cities[node->path[node->level-1]]->Longitude, cities[i]->Latitude, cities[i]->Longitude);
                    newNode->next = NULL;
                    if (rear != NULL) {
                        rear->next = newNode;
                    }
                    rear = newNode;
                    if (front == NULL) {
                        front = newNode;
                    }
                }
            }
        }
        free(node);
    }

    printf("Minimum distance: %f km\n", minDist);
    displayPath(head, minPath, n);
}

void loadCitiesFromCSV(const char* filename, struct City** head) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Could not open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        char name[50];
        float latitude, longitude;
        if (sscanf(line, "%[^,],%f,%f", name, &latitude, &longitude) == 3) {
            addCity(head, name, latitude, longitude);
        }
    }

    fclose(file);
}

int main() {
    struct City* head = NULL;
    loadCitiesFromCSV("cities.csv", &head);

    char start[50];
    printf("Enter starting point: ");
    scanf(" %[^\n]s", start);

    int cityCount = 0;
    struct City* current = head;
    while (current != NULL) {
        cityCount++;
        current = current->next;
    }

    int startIdx = -1;
    current = head;
    int idx = 0;
    while (current != NULL) {
        if (strcmp(current->Name, start) == 0) {
            startIdx = idx;
            break;
        }
        idx++;
        current = current->next;
    }

    if (startIdx == -1) {
        printf("Starting point not found in the list.\n");
        freeCityList(head);
        return 1;
    }

    int path[cityCount];
    
    clock_t start_time = clock();

    bfsTSP(head, path, cityCount, startIdx);

    clock_t end_time = clock();
    double elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Time elapsed: %f seconds\n", elapsed_time);

    freeCityList(head);
    return 0;
}
