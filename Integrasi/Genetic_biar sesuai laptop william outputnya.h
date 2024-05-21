#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define POPULATION_SIZE 100
#define TOURNAMENT_SELECTION_SIZE 5
#define MUTATION_RATE 0.1
#define CROSSOVER_RATE 0.9

typedef struct {
    char nama[50];
    double latitude;
    double longitude;
} Kota;

typedef struct {
    double jarak;
    Kota* rute;
} Kromosom;

double deg2rad(double deg) {
    return deg * (3.14159265359 / 180);
}

double hitungJarak(Kota kota1, Kota kota2) {
    double lat1 = deg2rad(kota1.latitude);
    double lon1 = deg2rad(kota1.longitude);
    double lat2 = deg2rad(kota2.latitude);
    double lon2 = deg2rad(kota2.longitude);

    double dlon = lon2 - lon1;
    double dlat = lat2 - lat1;

    double jarak = 2 * 6371 * asin(sqrt(pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2)));
    return jarak;
}

double hitungTotalJarak(Kota* rute, int panjangkota) {
    double total_jarak = 0;
    for (int i = 0; i < panjangkota - 1; i++) {
        total_jarak += hitungJarak(rute[i], rute[i + 1]);
    }
    total_jarak += hitungJarak(rute[panjangkota - 1], rute[0]);
    return total_jarak;
}

Kota* buatKota(char* filename, int* panjangkota) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("File tidak ditemukan.\n");
        exit(1);
    }

    int count = 0;
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        count++;
    }
    rewind(file);

    Kota* kota = (Kota*)malloc(count * sizeof(Kota));
    int index = 0;
    char namakota[50];
    double latitude, longitude;

    while (fscanf(file, "%49[^,],%lf,%lf\n", namakota, &latitude, &longitude) == 3) {
        strcpy(kota[index].nama, namakota);
        kota[index].latitude = latitude;
        kota[index].longitude = longitude;
        index++;
    }
    fclose(file);

    *panjangkota = count;
    return kota;
}

Kromosom* buatPopulasi(Kota* kota, int panjangkota, int size) {
    Kromosom* populasi = (Kromosom*)malloc(size * sizeof(Kromosom));

    for (int i = 0; i < size; i++) {
        Kota* rute = (Kota*)malloc(panjangkota * sizeof(Kota));
        memcpy(rute, kota, panjangkota * sizeof(Kota));
        for (int j = panjangkota - 1; j > 0; j--) { // change from 1 to 0
            int k = rand() % (j + 1); // change from 1 + rand() % j to rand() % (j + 1)
            Kota temp = rute[j];
            rute[j] = rute[k];
            rute[k] = temp;
        }
        double jarak = hitungTotalJarak(rute, panjangkota);
        populasi[i].jarak = jarak;
        populasi[i].rute = rute;
    }
    return populasi;
}


Kromosom selection(Kromosom* populasi, int size) {
    Kromosom best = populasi[rand() % size];
    for (int i = 1; i < TOURNAMENT_SELECTION_SIZE; i++) {
        Kromosom kandidat = populasi[rand() % size];
        if (kandidat.jarak < best.jarak) {
            best = kandidat;
        }
    }
    return best;
}

// Fungsi untuk melakukan crossover antara dua kromosom
void crossover(Kromosom parent1, Kromosom parent2, Kromosom* child1, Kromosom* child2, int panjangkota) {
    // Menentukan titik potong acak untuk crossover
    int point = 1 + rand() % (panjangkota - 1);
    int k1 = point, k2 = point;
    // Salin bagian awal rute dari parent1 ke child1 dan dari parent2 ke child2
    for (int i = 0; i < point; i++) {
        child1->rute[i] = parent1.rute[i];
        child2->rute[i] = parent2.rute[i];
    }
    // Mengisi bagian sisa rute untuk child1 dengan kota-kota dari parent2 dan memastikan tidak ada duplikasi
    // Mengisi bagian sisa rute untuk child2 dengan kota-kota dari parent1 dan memastikan tidak ada duplikasi
    for (int i = 1; i < panjangkota; i++) {
        int flag1 = 1, flag2 = 1;
        for (int j = 0; j < point; j++) {
            // Memeriksa apakah kota parent2 sudah ada di bagian awal child1
            if (parent2.rute[i].latitude == child1->rute[j].latitude && parent2.rute[i].longitude == child1->rute[j].longitude) {
                flag1 = 0;
            }
            // Memeriksa apakah kota parent1 sudah ada di bagian awal child2
            if (parent1.rute[i].latitude == child2->rute[j].latitude && parent1.rute[i].longitude == child2->rute[j].longitude) {
                flag2 = 0;
            }
        }
        // Jika kota dari parent2 belum ada di child1, tambahkan kota tersebut
        if (flag1 && k1 < panjangkota) {
            child1->rute[k1++] = parent2.rute[i];
        }
        // Jika kota dari parent1 belum ada di child2, tambahkan kota tersebut
        if (flag2 && k2 < panjangkota) {
            child2->rute[k2++] = parent1.rute[i];
        }
    }
    // Hitung jarak total untuk rute child1
    child1->jarak = hitungTotalJarak(child1->rute, panjangkota);
    // Hitung jarak total untuk rute child2
    child2->jarak = hitungTotalJarak(child2->rute, panjangkota);
}

// Fungsi untuk melakukan mutasi pada kromosom
void mutate(Kromosom* Kromosom, int panjangkota) {
    // Loop melalui setiap kota dalam rute, dimulai dari indeks 1 karena menghindari kota awal
    for (int i = 1; i < panjangkota; i++) {
        // Tentukan apakah akan melakukan mutasi berdasarkan MUTATION_RATE
        if ((double)rand() / RAND_MAX < MUTATION_RATE) {
            // Pilih dua titik acak dalam rute untuk ditukar
            int point1 = 1 + rand() % (panjangkota - 1);
            int point2 = 1 + rand() % (panjangkota - 1);
            // Tukar dua kota yang dipilih
            Kota temp = Kromosom->rute[point1];
            Kromosom->rute[point1] = Kromosom->rute[point2];
            Kromosom->rute[point2] = temp;
        }
    }
    // Hitung kembali total jarak rute setelah mutasi
    Kromosom->jarak = hitungTotalJarak(Kromosom->rute, panjangkota);
}


Kromosom geneticAlgorithm(Kota* kota, int panjangkota) {
    Kromosom* populasi = buatPopulasi(kota, panjangkota, POPULATION_SIZE);
    Kromosom best = populasi[0];
    int generasi = 0;

    while (generasi < 300) {
        Kromosom* new_populasi = (Kromosom*)malloc(POPULATION_SIZE * sizeof(Kromosom));

        new_populasi[0] = best;
        new_populasi[1] = selection(populasi, POPULATION_SIZE);

        for (int i = 2; i < POPULATION_SIZE; i += 2) {
            Kromosom parent1 = selection(populasi, POPULATION_SIZE);
            Kromosom parent2 = selection(populasi, POPULATION_SIZE);

            Kromosom child1 = { .rute = (Kota*)malloc(panjangkota * sizeof(Kota)) };
            Kromosom child2 = { .rute = (Kota*)malloc(panjangkota * sizeof(Kota)) };

            if ((double)rand() / RAND_MAX < CROSSOVER_RATE) {
                crossover(parent1, parent2, &child1, &child2, panjangkota);
            } else {
                memcpy(child1.rute, parent1.rute, panjangkota * sizeof(Kota));
                memcpy(child2.rute, parent2.rute, panjangkota * sizeof(Kota));
                child1.jarak = parent1.jarak;
                child2.jarak = parent2.jarak;
            }

            mutate(&child1, panjangkota);
            mutate(&child2, panjangkota);

            new_populasi[i] = child1;
            new_populasi[i + 1] = child2;
        }

        for (int i = 0; i < POPULATION_SIZE; i++) {
            if (&populasi[i] != &best) {
                // free(populasi[i].rute);
            }
        }
        free(populasi);
        populasi = new_populasi;
        best = populasi[0];

        for (int i = 1; i < POPULATION_SIZE; i++) {
            if (populasi[i].jarak < best.jarak) {
                best = populasi[i];
            }
        }

        generasi++;
    }

    free(populasi);

    return best;
}

int genetic(char filename[], char startingPoint[]) {
    printf("\nAlgortima Genetic:\n");
    srand(time(NULL));

    int panjangkota;
    Kota* kota = buatKota(filename, &panjangkota);

    int startIndex = -1;
    for (int i = 0; i < panjangkota; i++) {
        if (strcmp(kota[i].nama, startingPoint) == 0) {
            startIndex = i;
            break;
        }
    }
    if (startIndex != -1 && startIndex != 0) {
        Kota temp = kota[0];
        kota[0] = kota[startIndex];
        kota[startIndex] = temp;
    }

    if (startIndex == -1) {
        printf("Kota tidak ditemukan\n");
        return 1;
    }

    clock_t start = clock();
    Kromosom best = geneticAlgorithm(kota, panjangkota);
    clock_t end = clock();

    printf("Best rute found:\n");
    for (int i = 0; i < panjangkota; i++) {
        printf("%s -> ", kota[i].nama);
    }
    printf("%s\n", kota[0].nama);

    printf("Best rute distance: %.5f km\n", best.jarak);
    printf("Time elapsed: %.10f s\n", ((double)(end - start)) / CLOCKS_PER_SEC);

    free(best.rute);
    free(kota);
    return 0;
}
