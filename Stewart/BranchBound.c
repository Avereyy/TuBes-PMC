/*
Program Branch And Bound
Dibuat oleh : Stewart Chauniago
Sumber Ide yang digunakan dalam Branch And Bound ini :
1.  https://youtu.be/6Mrv7YjZ00I?si=uoUnCG5so-BfKW7V
2.  https://youtu.be/1FEP_sNb62k?si=6Ji9gfFYE8_mKNIt 
*/

/*
Penyebab adanya perbedaan hasil dengan algoritma lain akibat pada metode branch and bound yang telah dibuat
Lebih terfokus untuk mencari nilai-nilai minimum yang ada pada lokasi tersebut yang nantinya akan digabungkan
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

typedef struct Kota {
    char nama[50];
    double lintang;
    double bujur;
} Kota;

#define maksimal 100
#define INF 9999999.99 
// Mendeklarasikan Struct Dalam Bentuk Array karena pada kasus ini, nilai maksimal yang berada di file tidak akan lebih dari maksimal
struct Kota city[maksimal];

//Proses Data Dari File Ke Array
void filetoArray(struct Kota array[], int *jumlah, int *lokasi){
    FILE *address;
    char line[100];
    char file[100];
    char *token;
    int count = 0;
    // Program Meminta file yang akan digunakan
    printf("Enter list of cities file name: ");
    scanf("%s", file);
    address = fopen(file, "r");
    // Apabila file tidak valid
    if (address == NULL){
        printf("\nFile Tidak Ditemukan");
        exit(0);
    }
    // Dilakukan pengecekan apabila di dalam file ada isi atau tidak
    int isi = fgetc(address);
    if (isi == EOF){
        printf("Isi Dari File yang Dimasukkan Kosong");
        exit(0);
    }
    rewind(address); 
    // Mengambil Isi File menjadi bentuk array 
    while (fgets(line, 255, address)) {
        token = strtok(line, ",");
        strcpy(array[count].nama, token);
        token = strtok(NULL, ",");
        array[count].lintang = atof(token);
        token = strtok(NULL, ",");
        array[count].bujur = atof(token);
        count++;
    }
    // Menghitung banyak jumlah data yang terdapat pada bagian ini
    *jumlah = count;
    char input[20];
    int ketemu = 0; 
    // Memasukkan strating point yang akan digunakan 
    printf("Enter starting point: ");
    scanf("%s", input);
    // Melakukan pengubahan dari nama lokasi menjadi letak alamat ke-n
    for(int i= 0; i<count; i++){
        if(strcasecmp(city[i].nama, input) == 0){
            *lokasi = i;
            ketemu = 1; 
        }
    }
    // Jika tidak ditemukan nama tempat yang diinput maka akan membuat program berhenti akibat starting point tidak valid
    if (ketemu == 0){
        printf("Kota tidak Ditemukan. Silahkan Ulangi Lagi"); 
        exit(0); 
    }
    fclose(address);
}

// Mengubah radian ke derajat
double RadiantoDegree(double radian){
        return radian*(3.141592653/180);
}

// Menghitung Jarak antar kota
double hitungJarak(Kota kota1, Kota kota2) {
    double lintang1 = RadiantoDegree(kota1.lintang);
    double bujur1 = RadiantoDegree(kota1.bujur);
    double lintang2 = RadiantoDegree(kota2.lintang);
    double bujur2 = RadiantoDegree(kota2.bujur);
    double bujur = bujur2 - bujur1;
    double lintang = lintang2 - lintang1;
    double jarak = 2*6371*asin(sqrt(pow(sin(lintang / 2), 2) + cos(lintang1) * cos(lintang2) * pow(sin(bujur / 2), 2)));
    return jarak;
}

// Melakukan pengubahan terhadap array yang telah didapatkan menjadi matrix
void arraytoMatrix(struct Kota array[maksimal], double matrix[maksimal][maksimal], int jumlah){
    for(int i = 0; i < jumlah; i++){
        for(int j = 0; j < jumlah; j++){
            // Apabila merupakan jarak antar 1 kota
            if(i == j){
                matrix[i][j] = INF;
            }
            // Apabila merupakan jarak antar 2 kota
            else{
                matrix[i][j] = hitungJarak(array[i], array[j]);
            }
        }
    }
}

// Program Branch And Bound
// Untuk menyimpan data pergerakan pertama hingga selesai
int movement[maksimal] = {0};
// Untuk melakukan pengecekan apabila tempat tersebut telah dikunjungi atau belum
int visited[maksimal] = {0}; 
// Variable ini digunakan untuk membantu pada saat pergerakan
// Dengan adanya variable posisi ke ini maka akan membuat akan lebih mudah pada saat mencari pergerakan yang dilakukan 
int posisike = 0; 
double branchBound(int jumlah, double matrix[jumlah][jumlah], double j_total, int lokasi){
    double minimum = INF;
    int node, n = 0, loop=0;
    double nmatrix[jumlah][jumlah], mmatrix[jumlah][jumlah];
    double col[jumlah], row[jumlah];
    // Pada bagian lokasi yang didapatkan dijadikan nilai 1 karena pada bagian main, telah dilakukan reduksi matrix
    visited[lokasi] = 1;
    int i, j, k; 
    // Posisi yang saat ini berada pada lokasi yang saat ini
    movement[posisike] = lokasi;
    // Mencari penentuan lokasi selanjutnya yang akan dilewati
    posisike++;

    // Melakukan pengulangan sebanyak jumlah data 
    for(i=0; i<jumlah; i++){
        if(visited[i] != 1){
            // menghitung banyak rekursi yang telah dilakukan pada saat proses ini 
            loop++;

            // Melakukan pembuatan ulang matrix
            // Hal ini bertujuan supaya apabila pada bagian yang telah dikunjungi diubah semuanya menjadi infinity
            for(j=0; j<jumlah; j++){
                for(k=0; k<jumlah; k++){
                    if(j == lokasi || k == i || (k == lokasi && j == i) )
                        nmatrix[j][k] = INF;
                    else
                        nmatrix[j][k] = matrix[j][k]; 
                }
            }

            // Mencari nilai yang paling minimum diantara masing-masing baris
            for(j=0; j<jumlah; j++){
                row[j] = nmatrix[j][0];
                for(k=0; k<jumlah; k++){
                    if(nmatrix[j][k] != INF){
                        if(nmatrix[j][k] < row[j])
                            row[j] = nmatrix[j][k];
                    }
                }
                // Dilakukan pengecekan, apabila ternyata nilai yang paling kecil, maka nilainya akan ditambahkan nol
                // Sebaliknya, apabila itu bernilai, maka total jaraknya akan ditambahlan 
                if(row[j] == INF)
                    row[j] = 0;
                n += row[j];
            }

            // Dibuat supaya setiap Array yang ada di sana, dikurangi dengan nilai minimumnya pada masing-masing baris 
            if(n > 0){
                for(j=0; j<jumlah; j++){
                    for(k=0; k<jumlah; k++){
                        // Dilakukan pengecekan apabila nilainya infinite maka akan membuat suatu nilai infinite dikurangi dengan baris
                        if(nmatrix[j][k] != INF)
                            nmatrix[j][k] = nmatrix[j][k] - row[j];
                        else
                            nmatrix[j][k] = nmatrix[j][k];
                    }
                }
            }

            // Mencari nilai yang paling minimum diantara masing-masing kolom
            for(j=0; j<jumlah; j++){
                col[j] = nmatrix[0][j];
                for(k=0; k<jumlah; k++){
                    if(nmatrix[k][j] != INF){
                        if(nmatrix[k][j] < col[j])
                            col[j] = nmatrix[k][j];
                    }
                }
                // Dilakukan pengecekan, apabila ternyata nilai yang paling kecil, maka nilainya akan ditambahkan nol
                // Sebaliknya, apabila itu bernilai, maka total jaraknya akan ditambahlan 
                if(col[j] == INF)
                    col[j] = 0;
                n += col[j];
            }

            // Dibuat supaya setiap Array yang ada di sana, dikurangi dengan nilai minimumnya pada masing-masing baris 
            if(n > 0){ 
                for(j=0; j<jumlah; j++){
                    for(k=0; k<jumlah; k++){
                        if(nmatrix[j][k] != INF){
                            // Dilakukan pengecekan apabila nilainya infinite maka akan membuat suatu nilai infinite dikurangi dengan kolom
                            nmatrix[j][k] = nmatrix[j][k] - col[k];
                        }
                        else
                            nmatrix[j][k] = nmatrix[j][k];
                    }
                }
            }
            // Dibandingkan nilai minimum yang didapatkan dengan data-data sebelumnya
            if(minimum > j_total + matrix[lokasi][i] + n){
                // Jika iya, maka data digantikan dengan data yang minimumnya lebih kecil
                minimum = j_total + matrix[lokasi][i] + n;
                node = i;
                for(j=0; j<jumlah; j++){
                    for(k=0; k<jumlah; k++){
                        mmatrix[j][k] = nmatrix[j][k];
                    }
                }
            }
        }
        n = 0;
    } 
    // Apabila hanya 1 kota yang belum dikunjungi
    if(loop < 2){
        movement[posisike] = node;
        return minimum;
    }
    else{
        // Jika, tidak maka akan dilakukan rekursi
        j_total = branchBound(jumlah, mmatrix, minimum, node);
    }
}


void Hasil(double jarak[maksimal][maksimal], int jumlah, int lokasi){
    double r_matrix[jumlah][jumlah], col[jumlah], row[jumlah];
    double total_langkah = 0.0;
    int i, j, k;
    clock_t start = clock();

    // Menentukan nilai pada baris yang paling kecil
    for(i=0; i<jumlah; i++){
        row[i] = jarak[i][0]; 
        for(j=0; j<jumlah;j++){
            if(jarak[i][j] != INF){
                if(jarak[i][j] < row[i]){
                    row[i] = jarak[i][j]; 
                }
            }
        }
        // Nilai pada kecil tersebut dijumlahkan ke nilai jumlah jarak
        total_langkah += row[i];  
    }

    // Dilakukan pengurangan terhadap baris dengan dengan baris yang terkecil pada masing-masing baris tersebut
    for(i =0; i<jumlah; i++){
        for(j=0; j<jumlah; j++){
            if(jarak[i][j] != INF)
                r_matrix[i][j] = jarak[i][j] - row[i]; 
            else
                r_matrix[i][j] = jarak[i][j]; 
        }
    } 

    // Menentukan nilai pada kolom yang paling kecil
    for(i=0; i<jumlah; i++){
        col[i] = r_matrix[0][i]; 
        for(j=0; j<jumlah;j++){
            if(r_matrix[j][i] != INF){
                if(r_matrix[j][i] < col[i]){
                    col[i] = r_matrix[j][i]; 
                }
            }
        }
        // Nilai pada kecil tersebut dijumlahkan ke nilai jumlah jarak
        total_langkah += col[i];  
    }

    // Dilakukan pengurangan terhadap baris dengan dengan baris yang terkecil pada masing-masing baris tersebut
    for(i =0; i<jumlah; i++){
        for(j=0; j<jumlah; j++){
            if(r_matrix[i][j] != INF){
                r_matrix[i][j] = r_matrix[i][j] - col[j]; 
            }
            else{
                r_matrix[i][j] = r_matrix[i][j];
            }
        }
    }

    // Menentukan jumlah total langkah yang didapatkan
    total_langkah = branchBound(jumlah, r_matrix, total_langkah, lokasi);
    clock_t end = clock(); 
    
    // Mencetak path yang dilewati oleh pengguna dari Kota Awal ke Kota Awal lagi 
    for(i=0; i < jumlah; i ++){
        printf("%s -> ", city[movement[i]].nama); 
    }
    printf(" %s\n", city[lokasi].nama); 

    // Mencetak jumlah jarak yang dilewati oleh pengguna
    printf("Jumlah Jarak : %.10f km", total_langkah);
    double time_use = ((double) (end-start)) /CLOCKS_PER_SEC;
    // Mencetak waktu yang diperlukan oleh komputer
    printf("\nWaktu yang digunakan : %.10f s", time_use); 
}

int main(){ 
    double jarak[maksimal][maksimal];
    int jumlah, lokasi;

    // Penggunaan sub fungsi yang telah dibuat
    filetoArray(city, &jumlah, &lokasi);
    arraytoMatrix(city, jarak, jumlah);
    Hasil(jarak, jumlah, lokasi); 
}