#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_KOTA 100
#define MAKS_NAMA_PANJANG 50

typedef struct {
    char nama[MAKS_NAMA_PANJANG];
    double lintang;
    double bujur;
} Kota;

double rumusHaversine(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371000; // Radius bumi dalam meter
    double phi1 = lat1 * M_PI / 180.0;
    double phi2 = lat2 * M_PI / 180.0;
    double deltaPhi = (lat2 - lat1) * M_PI / 180.0;
    double deltaLambda = (lon2 - lon1) * M_PI / 180.0;

    double a = sin(deltaPhi / 2) * sin(deltaPhi / 2) +
               cos(phi1) * cos(phi2) *
               sin(deltaLambda / 2) * sin(deltaLambda / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    double jarak = R * c; // Meter
    return jarak / 1000.0; // Kilometer
}

void DFS(int kotaSaatIni, int dikunjungi[], double jarakSaatIni, double *jarakMin, int jalur[], int kedalaman, int jalurTerbaik[], Kota kota[], int jumlahKota) {
    dikunjungi[kotaSaatIni] = 1;
    jalur[kedalaman] = kotaSaatIni;

    if (kedalaman == jumlahKota - 1) { // Setelah semua dikunjungi, kembali ke kota awal
        jarakSaatIni += rumusHaversine(kota[kotaSaatIni].lintang, kota[kotaSaatIni].bujur, kota[jalur[0]].lintang, kota[jalur[0]].bujur);
        if (jarakSaatIni < *jarakMin) {
            *jarakMin = jarakSaatIni;
            memcpy(jalurTerbaik, jalur, jumlahKota * sizeof(int));
        }
        dikunjungi[kotaSaatIni] = 0; // Batal pengunjungan
        return;
    }

    for (int i = 0; i < jumlahKota; i++) { // Kunjungi kota yang belum dikunjungi
        if (!dikunjungi[i]) {
            double jarakBaru = jarakSaatIni + rumusHaversine(kota[kotaSaatIni].lintang, kota[kotaSaatIni].bujur, kota[i].lintang, kota[i].bujur);
            DFS(i, dikunjungi, jarakBaru, jarakMin, jalur, kedalaman + 1, jalurTerbaik, kota, jumlahKota);
        }
    }

    dikunjungi[kotaSaatIni] = 0;
}

int bacaKotaCSV(const char *filename, Kota kota[]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Tidak dapat membuka file");
        return -1;
    }

    int count = 0;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (count >= MAX_KOTA) {
            printf("Jumlah kota melebihi batas maksimum.\n");
            break;
        }
        sscanf(line, "%49[^,],%lf,%lf", kota[count].nama, &kota[count].lintang, &kota[count].bujur);
        count++;
    }

    fclose(file);
    return count;
}

void cetakSemuaJarak(Kota kota[], int jumlahKota) {
    printf("Jarak antara kota-kota:\n");
    printf("%-15s", "Kota");
    for (int i = 0; i < jumlahKota; i++) {
        printf("%-15s", kota[i].nama);
    }
    printf("\n");

    for (int i = 0; i < jumlahKota; i++) {
        printf("%-15s", kota[i].nama);
        for (int j = 0; j < jumlahKota; j++) {
            if (i == j) {
                printf("%-15s", "0.00");
            } else {
                double jarak = rumusHaversine(kota[i].lintang, kota[i].bujur, kota[j].lintang, kota[j].bujur);
                printf("%-15.2f", jarak);
            }
        }
        printf("\n");
    }
    printf("\n");
}

int main() {
    Kota kota[MAX_KOTA];
    const char *filename = "kota_05.csv";
    int jumlahKota = bacaKotaCSV(filename, kota);

    if (jumlahKota <= 0) {
        printf("Tidak ada kota untuk diproses.\n");
        return 1;
    }

    cetakSemuaJarak(kota, jumlahKota);

    int dikunjungi[MAX_KOTA] = {0};
    int jalur[MAX_KOTA];
    int jalurTerbaik[MAX_KOTA];
    double jarakMin = INFINITY;

    struct timespec start, end; // Waktu DFS
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Mulai DFS dari kota pertama (indeks 0)
    DFS(0, dikunjungi, 0, &jarakMin, jalur, 0, jalurTerbaik, kota, jumlahKota);

    clock_gettime(CLOCK_MONOTONIC, &end);

    double waktu_diperlukan = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Jalur terpendek: ");
    double totalJarak = 0.0;
    for (int i = 0; i < jumlahKota; i++) {
        int dari = jalurTerbaik[i];
        int ke = jalurTerbaik[(i + 1) % jumlahKota];
        double jarak = rumusHaversine(kota[dari].lintang, kota[dari].bujur, kota[ke].lintang, kota[ke].bujur);
        totalJarak += jarak;

        printf("%s", kota[dari].nama);
        if (i < jumlahKota - 1) {
            printf(" -> ");
        }
    }
    printf(" -> %s\n", kota[jalurTerbaik[0]].nama); // Menambah jarak ke kota tujuan akhir
    printf("Jarak: %.2f km\n", jarakMin);
    printf("Jarak terperinci: ");
    for (int i = 0; i < jumlahKota; i++) {
        int dari = jalurTerbaik[i];
        int ke = jalurTerbaik[(i + 1) % jumlahKota];
        double jarak = rumusHaversine(kota[dari].lintang, kota[dari].bujur, kota[ke].lintang, kota[ke].bujur);
        printf("%.2f", jarak);
        if (i < jumlahKota - 1) {
            printf(" + ");
        } else {
            printf(" = %.2f km\n", totalJarak);
        }
    }
    printf("Waktu yang dibutuhkan: %.9f detik\n", waktu_diperlukan);

    return 0;
}
