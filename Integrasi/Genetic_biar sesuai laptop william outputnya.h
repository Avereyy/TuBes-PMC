#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define populasi_SIZE 100
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

// Fungsi untuk mengonversi derajat ke radian
double deg2rad(double deg) {
    return deg * (3.14159265359 / 180);
}

// Fungsi untuk menghitung jarak antara dua kota menggunakan rumus Haversine
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

// Fungsi untuk menghitung total jarak dari rute yang diberikan
double hitungTotalJarak(Kota* rute, int panjangkota) {
    double total_jarak = 0;
    // Loop untuk menghitung jarak antara setiap pasangan kota dalam rute
    for (int i = 0; i < panjangkota - 1; i++) {
        // Tambahkan jarak antara kota i dan kota i+1 ke total_jarak
        total_jarak += hitungJarak(rute[i], rute[i + 1]);
    }
    // Tambahkan jarak antara kota terakhir dalam rute dengan kota pertama untuk membentuk siklus
    total_jarak += hitungJarak(rute[panjangkota - 1], rute[0]);
    // Kembalikan total jarak rute
    return total_jarak;
}

// Fungsi untuk membaca daftar kota dari file
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

// Fungsi untuk membuat populasi awal dari kromosom
Kromosom* buatPopulasi(Kota* kota, int panjangkota, int size) {
     // Alokasikan memori untuk array populasi dengan ukuran 'size'
    Kromosom* populasi = (Kromosom*)malloc(size * sizeof(Kromosom));

    for (int i = 0; i < size; i++) {
        // Alokasikan memori untuk rute kota untuk setiap kromosom
        Kota* rute = (Kota*)malloc(panjangkota * sizeof(Kota));
        // Salin daftar kota awal ke dalam rute untuk kromosom ini
        memcpy(rute, kota, panjangkota * sizeof(Kota));
         // Mengacak urutan kota-kota dalam rute
        for (int j = panjangkota - 1; j > 1; j--) {
            int k = 1 + rand() % j; // Pilih indeks acak antara 1 dan j
             // Tukar kota di indeks j dengan kota di indeks k
            Kota temp = rute[j];
            rute[j] = rute[k];
            rute[k] = temp;
        }
        // Hitung total jarak untuk rute yang telah diacak
        double jarak = hitungTotalJarak(rute, panjangkota);
        // Simpan jarak dan rute dalam kromosom di populasi
        populasi[i].jarak = jarak;
        populasi[i].rute = rute;
    }
    // Kembalikan pointer ke array populasi yang berisi kromosom
    return populasi;
}

// Fungsi untuk melakukan seleksi turnamen
Kromosom selection(Kromosom* populasi, int size) {
    // Inisialisasi kromosom terbaik dengan memilih secara acak dari populasi
    Kromosom best = populasi[rand() % size];
    // Melakukan seleksi turnamen sebanyak TOURNAMENT_SELECTION_SIZE kali
    for (int i = 1; i < TOURNAMENT_SELECTION_SIZE; i++) {
        // Memilih kandidat secara acak dari populasi
        Kromosom kandidat = populasi[rand() % size];
        // Membandingkan jarak kandidat dengan jarak kromosom terbaik saat ini
        if (kandidat.jarak < best.jarak) {
            // Jika jarak kandidat lebih kecil, maka kandidat menjadi kromosom terbaik baru
            best = kandidat;
        }
    }
    // Mengembalikan kromosom terbaik yang ditemukan selama seleksi turnamen
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

// Fungsi utama algoritma genetika untuk menemukan rute terbaik
Kromosom geneticAlgorithm(Kota* kota, int panjangkota) {
    Kromosom* populasi = buatPopulasi(kota, panjangkota, populasi_SIZE);

    // Anggap kromosom pertama dalam populasi sebagai yang terbaik awalnya
    Kromosom best = populasi[0];
    int generasi = 0;

    while (generasi < 300) {
        Kromosom* new_populasi = (Kromosom*)malloc(populasi_SIZE * sizeof(Kromosom));

        // Pertahankan solusi terbaik yang ditemukan sejauh ini
        new_populasi[0] = best;
        // Seleksi kromosom terbaik kedua
        new_populasi[1] = selection(populasi, populasi_SIZE);

        // Lakukan operasi crossover dan mutasi untuk menghasilkan populasi baru
        for (int i = 2; i < populasi_SIZE; i += 2) {
            // Pilih dua induk melalui seleksi turnamen
            Kromosom parent1 = selection(populasi, populasi_SIZE);
            Kromosom parent2 = selection(populasi, populasi_SIZE);

            // Alokasikan memori untuk dua anak
            Kromosom child1 = { .rute = (Kota*)malloc(panjangkota * sizeof(Kota)) };
            Kromosom child2 = { .rute = (Kota*)malloc(panjangkota * sizeof(Kota)) };

            if ((double)rand() / RAND_MAX < CROSSOVER_RATE) {
                // Lakukan crossover dengan probabilitas tertentu
                crossover(parent1, parent2, &child1, &child2, panjangkota);
            } else {
                // Jika tidak crossover, anak-anak adalah salinan dari induk
                memcpy(child1.rute, parent1.rute, panjangkota * sizeof(Kota));
                memcpy(child2.rute, parent2.rute, panjangkota * sizeof(Kota));
                child1.jarak = parent1.jarak;
                child2.jarak = parent2.jarak;
            }

            mutate(&child1, panjangkota);
            mutate(&child2, panjangkota);

            // Masukkan anak-anak ke dalam populasi baru
            new_populasi[i] = child1;
            new_populasi[i + 1] = child2;
        }

        // Bebaskan memori dari populasi lama
        free(populasi);
        // Set populasi baru sebagai populasi saat ini
        populasi = new_populasi;
        best = populasi[0];
        // Cari kromosom terbaik dalam populasi baru
        for (int i = 1; i < populasi_SIZE; i++) {
            if (populasi[i].jarak < best.jarak) {
                best = populasi[i];
            }
        }
        // looping sampai generasi sesuai yang diinginkan
        generasi++;
    }

    // Bebaskan memori dari kromosom yang bukan yang terbaik
    for (int i = 0; i < populasi_SIZE; i++) {
        if (&populasi[i] != &best) {
            // free(populasi[i].rute);
        }
    }
    // Bebaskan memori dari populasi
    // free(populasi);

    // Kembalikan kromosom terbaik yang ditemukan
    return best;
}

int genetic(char filename[],char startingPoint[]) {
    printf("\nAlgortima Genetic:\n");
    srand(time(NULL)); // Membuat seed untuk fungsi rand() berdasarkan waktu saat ini


    int panjangkota;
    Kota* kota = buatKota(filename, &panjangkota); // Memanggil fungsi buatKota untuk membuat array kota dari file yang diberikan

    int startIndex = -1; // Inisialisasi indeks titik awal ke -1 (belum ditemukan)
    for (int i = 0; i < panjangkota; i++) {
        if (strcmp(kota[i].nama, startingPoint) == 0) {
            startIndex = i; // Jika nama kota sesuai dengan titik awal, simpan indeksnya
            break;
        }
    }
    if (startIndex != -1 && startIndex != 0) {
        // Tukar posisi kota titik awal dengan kota pertama
        Kota temp = kota[0];
        kota[0] = kota[startIndex];
        kota[startIndex] = temp; 
    }

    if (startIndex == -1) { // Jika titik awal tidak ditemukan dalam daftar kota
        printf("Kota tidak ditemukan\n");
        return 1;
    }

    clock_t start = clock();
    Kromosom best = geneticAlgorithm(kota, panjangkota);
    clock_t end = clock();

    printf("Best rute found:\n");
    for (int i = 0; i < panjangkota; i++) {
        printf("%s -> ", best.rute[i].nama); // Menampilkan rute terbaik yang ditemukan
    }
    printf("%s\n", best.rute[0].nama); // Menampilkan kembali titik awal untuk menunjukkan siklus tertutup

    printf("Best rute distance: %.5f km\n", best.jarak);
    printf("Time elapsed: %.10f s\n", ((double)(end - start)) / CLOCKS_PER_SEC);

    // free(best.rute);
    free(kota);
    return 0;
}
