#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <stdbool.h>
#define M_PI 3.14159265358979323846
#define R 6371

struct Kota {
    char Nama[50];
    float Lattitude;
    float Longitude;
    struct Kota* next;
};

void insert(struct Kota** head, char* new_nama, float new_lattitude, float new_longitude) {
    struct Kota* new_Kota = (struct Kota*)malloc(sizeof(struct Kota));
    strcpy(new_Kota->Nama, new_nama);
    new_Kota->Lattitude = new_lattitude;
    new_Kota->Longitude = new_longitude;
    new_Kota->next = *head;
    *head = new_Kota;
}

void freeList(struct Kota* head) {
    struct Kota* tmp;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

float distance(float lat1, float lon1, float lat2, float lon2) {
    float dlat = (lat2 - lat1) * M_PI / 180.0;
    float dlon = (lon2 - lon1) * M_PI / 180.0;
    float a = sin(dlat / 2) * sin(dlat / 2) + cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) * sin(dlon / 2) * sin(dlon / 2);
    float c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return R * c;
}

void printPath(struct Kota* head, int path[], int n) {
    struct Kota* cities[n];
    struct Kota* current = head;
    for (int i = 0; i < n; i++) {
        cities[i] = current;
        current = current->next;
    }
    for (int i = 0; i < n; i++) {
        printf("%s -> ", cities[path[i]]->Nama);
    }
    printf("%s\n", cities[path[0]]->Nama);
}

void bfs(struct Kota *head, int path[], int n, int startIdx) {
    float minDist = FLT_MAX;
    int minPath[n];

    struct Kota* cities[n];
    struct Kota* current = head;
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
            node->cost += distance(cities[node->path[n-1]]->Lattitude, cities[node->path[n-1]]->Longitude, cities[node->path[0]]->Lattitude, cities[node->path[0]]->Longitude);
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
                    newNode->cost = node->cost + distance(cities[node->path[node->level-1]]->Lattitude, cities[node->path[node->level-1]]->Longitude, cities[i]->Lattitude, cities[i]->Longitude);
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
    printPath(head, minPath, n);
}

int main() {
    char filename[50];
    char start[50];

    printf("Enter list of cities file name: ");
    scanf("%s", filename);

    struct Kota* head = NULL;
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        printf("Failed to open file %s. Make sure the file exists.\n", filename);
        return 1;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char nama[50];
        float Lattitude, Longitude;

        char* token = strtok(line, ",");
        strcpy(nama, token);

        token = strtok(NULL, ",");
        Lattitude = atof(token);

        token = strtok(NULL, ",");
        Longitude = atof(token);

        insert(&head, nama, Lattitude, Longitude);
    }
    fclose(file);

    printf("Enter starting point: ");
    scanf(" %[^\n]s", start);

    int n = 0;
    struct Kota* current = head;
    while (current != NULL) {
        n++;
        current = current->next;
    }

    int startIdx = -1;
    current = head;
    int idx = 0;
    while (current != NULL) {
        if (strcmp(current->Nama, start) == 0) {
            startIdx = idx;
            break;
        }
        idx++;
        current = current->next;
    }

    if (startIdx == -1) {
        printf("Starting point not found in the list.\n");
        freeList(head);
        return 1;
    }

    int path[n];
    bfs(head, path, n, startIdx);

    freeList(head);
    return 0;
}
