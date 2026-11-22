#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <limits>
#include <climits>
using namespace std;

// =====================================
// 1. ARRAY STATIS
// =====================================

const int JUM_GIZI = 5;

string namaGizi[JUM_GIZI] = {
    "Karbohidrat", "Protein", "Lemak",
    "Sayuran", "Buah"
};

float standar[JUM_GIZI] = {150, 60, 20, 50, 50};

const float BATAS_BIAYA = 15000;


// =====================================
// 2. ENUM
// =====================================

enum Jenjang {
    SD = 1,
    SMP,
    SMA,
    POSYANDU
};


// =====================================
// 3. UNION
// =====================================

union Identitas {
    int npsn;
    int kodePosyandu;
};


// =====================================
// 4. STRUCT
// =====================================

struct Penerima {
    string resi;
    string nama;

    Jenjang jenjang;
    Identitas id;

    int porsi;
    float biaya;

    float gramasi[JUM_GIZI];
    float skor;
    bool masalah;
};


// =====================================
// 5. ARRAY DINAMIS DAN POINTER
// =====================================

Penerima* dataPenerima = nullptr;
int jumlah = 0;


// =====================================
// FUNGSI BANTU INPUT (VALIDASI)
// Mencegah program macet (infinite loop)
// jika pengguna mengetik huruf pada input angka
// =====================================

void bersihkanInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int bacaInt(const string& pesan, int minimal, int maksimal) {
    int x;
    while (true) {
        cout << pesan;
        if (cin >> x && x >= minimal && x <= maksimal) {
            bersihkanInput();
            return x;
        }
        bersihkanInput();
        cout << "Input tidak valid! Masukkan angka "
             << minimal << " - " << maksimal << ".\n";
    }
}

float bacaFloat(const string& pesan, float minimal) {
    float x;
    while (true) {
        cout << pesan;
        if (cin >> x && x >= minimal) {
            bersihkanInput();
            return x;
        }
        bersihkanInput();
        cout << "Input tidak valid! Minimal " << minimal << ".\n";
    }
}


// =====================================
// FUNGSI MENGHITUNG SKOR GIZI
// =====================================

float hitungGizi(const float gramasi[]) {

    float total = 0;

    for (int i = 0; i < JUM_GIZI; i++) {

        float nilai = (gramasi[i] / standar[i]) * 100;

        if (nilai > 100) {
            nilai = 100;
        }

        total += nilai;
    }

    return total / JUM_GIZI;
}


// =====================================
// FUNGSI MEMERIKSA MASALAH
// (pakai const reference, tidak menyalin struct)
// =====================================

bool cekMasalah(const Penerima& p) {

    for (int i = 0; i < JUM_GIZI; i++) {
        if (p.gramasi[i] < standar[i]) {
            return true;
        }
    }

    if (p.biaya > BATAS_BIAYA) {
        return true;
    }

    return false;
}


// =====================================
// FUNGSI MENAMPILKAN JENJANG
// =====================================

string namaJenjang(Jenjang j) {

    switch (j) {
        case SD: return "SD";
        case SMP: return "SMP";
        case SMA: return "SMA";
        case POSYANDU: return "Posyandu";
        default: return "Tidak diketahui";
    }
}


// Mengecek apakah nomor resi sudah ada
bool resiSudahAda(const string& resi) {
    for (int i = 0; i < jumlah; i++) {
        if (dataPenerima[i].resi == resi) {
            return true;
        }
    }
    return false;
}


// =====================================
// MENU 1: MENAMBAH DATA
// =====================================

void tambahData() {

    Penerima baru;

    cout << "\n=== INPUT DATA MBG ===\n";

    // Nomor resi harus unik (penting untuk binary search)
    while (true) {
        cout << "Nomor Resi: ";
        cin >> baru.resi;
        bersihkanInput();

        if (resiSudahAda(baru.resi)) {
            cout << "Nomor resi sudah terdaftar!\n";
        } else {
            break;
        }
    }

    do {
        cout << "Nama Sekolah/Instansi: ";
        getline(cin, baru.nama);

        if (baru.nama.empty()) {
            cout << "Nama tidak boleh kosong!\n";
        }
    } while (baru.nama.empty());

    cout << "\nPilih Jenjang:\n";
    cout << "1. SD\n";
    cout << "2. SMP\n";
    cout << "3. SMA\n";
    cout << "4. Posyandu\n";

    int pilih = bacaInt("Pilihan: ", 1, 4);
    baru.jenjang = static_cast<Jenjang>(pilih);

    if (baru.jenjang == POSYANDU) {
        baru.id.kodePosyandu = bacaInt("Kode Posyandu: ", 0, INT_MAX);
    } else {
        baru.id.npsn = bacaInt("NPSN: ", 0, INT_MAX);
    }

    baru.porsi = bacaInt("Jumlah Porsi: ", 1, INT_MAX);

    baru.biaya = bacaFloat("Biaya per Porsi: Rp", 0);

    cout << "\n=== INPUT GRAMASI AKTUAL ===\n";

    for (int i = 0; i < JUM_GIZI; i++) {
        baru.gramasi[i] = bacaFloat(namaGizi[i] + " (gram): ", 0);
    }

    baru.skor = hitungGizi(baru.gramasi);
    baru.masalah = cekMasalah(baru);

    // Memperbesar array dinamis
    Penerima* temp = new Penerima[jumlah + 1];

    for (int i = 0; i < jumlah; i++) {
        temp[i] = dataPenerima[i];
    }

    temp[jumlah] = baru;

    delete[] dataPenerima;
    dataPenerima = temp;
    jumlah++;

    cout << "\nData berhasil ditambahkan!\n";
    cout << "Skor Gizi: " << fixed << setprecision(2)
         << baru.skor << "%\n";

    if (baru.masalah) {
        cout << "Status: PERLU DIPERIKSA\n";
    } else {
        cout << "Status: SESUAI STANDAR\n";
    }
}


// =====================================
// MENU 2: MENAMPILKAN DATA
// =====================================

void tampilData() {

    if (jumlah == 0) {
        cout << "\nBelum ada data!\n";
        return;
    }

    cout << "\n=== DATA PENERIMA MBG ===\n";

    for (int i = 0; i < jumlah; i++) {

        cout << "\nData ke-" << i + 1;
        cout << "\nResi: " << dataPenerima[i].resi;
        cout << "\nNama: " << dataPenerima[i].nama;
        cout << "\nJenjang: " << namaJenjang(dataPenerima[i].jenjang);

        if (dataPenerima[i].jenjang == POSYANDU) {
            cout << "\nKode Posyandu: " << dataPenerima[i].id.kodePosyandu;
        } else {
            cout << "\nNPSN: " << dataPenerima[i].id.npsn;
        }

        cout << "\nJumlah Porsi: " << dataPenerima[i].porsi;
        cout << "\nBiaya per Porsi: Rp"
             << fixed << setprecision(0) << dataPenerima[i].biaya;

        cout << "\nSkor Gizi: "
             << fixed << setprecision(2)
             << dataPenerima[i].skor << "%";

        cout << "\nStatus: ";

        if (dataPenerima[i].masalah) {
            cout << "PERLU DIPERIKSA\n";
        } else {
            cout << "SESUAI STANDAR\n";
        }

        cout << "-----------------------------\n";
    }
}


// =====================================
// MENU 3: INSERTION SORT
// PAKET BERMASALAH DI URUTAN ATAS
// =====================================

void urutMasalah() {

    if (jumlah == 0) {
        cout << "\nData masih kosong!\n";
        return;
    }

    for (int i = 1; i < jumlah; i++) {

        Penerima kunci = dataPenerima[i];
        int j = i - 1;

        while (j >= 0 && kunci.masalah && !dataPenerima[j].masalah) {
            dataPenerima[j + 1] = dataPenerima[j];
            j--;
        }

        dataPenerima[j + 1] = kunci;
    }

    cout << "\nPaket bermasalah berhasil diprioritaskan!\n";

    tampilData();
}


// =====================================
// MENU 4: SELECTION SORT
// PORSI TERBANYAK DI URUTAN ATAS
// =====================================

void urutPorsi() {

    if (jumlah == 0) {
        cout << "\nData masih kosong!\n";
        return;
    }

    for (int i = 0; i < jumlah - 1; i++) {

        int terbesar = i;

        for (int j = i + 1; j < jumlah; j++) {
            if (dataPenerima[j].porsi > dataPenerima[terbesar].porsi) {
                terbesar = j;
            }
        }

        if (terbesar != i) {
            Penerima temp = dataPenerima[i];
            dataPenerima[i] = dataPenerima[terbesar];
            dataPenerima[terbesar] = temp;
        }
    }

    cout << "\nData berhasil diurutkan berdasarkan porsi!\n";

    tampilData();
}


// =====================================
// BUBBLE SORT RESI (tanpa menampilkan)
// Dipisah agar bisa dipakai binary search
// tanpa mencetak seluruh data
// =====================================

void sortResi() {

    for (int i = 0; i < jumlah - 1; i++) {

        bool tukar = false;

        for (int j = 0; j < jumlah - i - 1; j++) {

            if (dataPenerima[j].resi > dataPenerima[j + 1].resi) {

                Penerima temp = dataPenerima[j];
                dataPenerima[j] = dataPenerima[j + 1];
                dataPenerima[j + 1] = temp;
                tukar = true;
            }
        }

        if (!tukar) break;   // sudah terurut
    }
}


// =====================================
// MENU 5: URUTKAN NOMOR RESI
// =====================================

void urutResi() {

    if (jumlah == 0) {
        cout << "\nData masih kosong!\n";
        return;
    }

    sortResi();

    cout << "\nNomor resi berhasil diurutkan!\n";

    tampilData();
}


// =====================================
// MENU 6: BINARY SEARCH
// MENCARI NOMOR RESI
// =====================================

void cariResi() {

    if (jumlah == 0) {
        cout << "\nData masih kosong!\n";
        return;
    }

    // Binary search butuh data terurut
    sortResi();

    string cari;

    cout << "\nMasukkan nomor resi: ";
    cin >> cari;
    bersihkanInput();

    int kiri = 0;
    int kanan = jumlah - 1;

    bool ketemu = false;

    while (kiri <= kanan) {

        int tengah = kiri + (kanan - kiri) / 2;

        if (dataPenerima[tengah].resi == cari) {

            cout << "\nData ditemukan!\n";
            cout << "Nama: " << dataPenerima[tengah].nama;
            cout << "\nPorsi: " << dataPenerima[tengah].porsi;
            cout << "\nSkor Gizi: " << fixed << setprecision(2)
                 << dataPenerima[tengah].skor << "%\n";

            ketemu = true;
            break;
        }
        else if (dataPenerima[tengah].resi < cari) {
            kiri = tengah + 1;
        }
        else {
            kanan = tengah - 1;
        }
    }

    if (!ketemu) {
        cout << "\nNomor resi tidak ditemukan!\n";
    }
}


// =====================================
// MENU 7: LINEAR SEARCH
// MENCARI BERDASARKAN JENJANG
// =====================================

void cariJenjang() {

    if (jumlah == 0) {
        cout << "\nData masih kosong!\n";
        return;
    }

    cout << "\n1. SD\n";
    cout << "2. SMP\n";
    cout << "3. SMA\n";
    cout << "4. Posyandu\n";

    int pilih = bacaInt("Pilih jenjang: ", 1, 4);

    bool ketemu = false;

    for (int i = 0; i < jumlah; i++) {

        if (dataPenerima[i].jenjang == static_cast<Jenjang>(pilih)) {

            cout << "\nNama: " << dataPenerima[i].nama;
            cout << "\nResi: " << dataPenerima[i].resi;
            cout << "\nPorsi: " << dataPenerima[i].porsi << "\n";
            cout << "-----------------------------";

            ketemu = true;
        }
    }

    if (!ketemu) {
        cout << "\nData tidak ditemukan!";
    }

    cout << "\n";
}


// =====================================
// MENU 8: FILE I/O
// MENYIMPAN LAPORAN TXT
// =====================================

void simpanLaporan() {

    if (jumlah == 0) {
        cout << "\nData masih kosong, tidak ada yang disimpan!\n";
        return;
    }

    ofstream file("Laporan_MBG.txt");

    if (!file.is_open()) {
        cout << "Gagal menyimpan laporan!\n";
        return;
    }

    file << "=== LAPORAN MONITORING MBG ===\n";

    for (int i = 0; i < jumlah; i++) {

        file << "\nNomor Resi: " << dataPenerima[i].resi;
        file << "\nNama: " << dataPenerima[i].nama;
        file << "\nJenjang: " << namaJenjang(dataPenerima[i].jenjang);

        if (dataPenerima[i].jenjang == POSYANDU) {
            file << "\nKode Posyandu: " << dataPenerima[i].id.kodePosyandu;
        } else {
            file << "\nNPSN: " << dataPenerima[i].id.npsn;
        }

        file << "\nJumlah Porsi: " << dataPenerima[i].porsi;

        file << "\nBiaya per Porsi: Rp"
             << fixed << setprecision(0) << dataPenerima[i].biaya;

        file << "\nSkor Gizi: "
             << fixed << setprecision(2) << dataPenerima[i].skor << "%";

        if (dataPenerima[i].masalah) {
            file << "\nStatus: PERLU DIPERIKSA\n";
        } else {
            file << "\nStatus: SESUAI STANDAR\n";
        }

        file << "-------------------------\n";
    }

    file.close();

    cout << "\nLaporan berhasil disimpan!\n";
}


// =====================================
// PROGRAM UTAMA
// =====================================

int main() {

    int pilih;

    do {

        cout << "\n================================\n";
        cout << "   SISTEM MONITORING MBG\n";
        cout << "================================\n";

        cout << "1. Tambah Data Penerima\n";
        cout << "2. Tampilkan Data\n";
        cout << "3. Prioritas Paket Bermasalah\n";
        cout << "4. Urutkan Berdasarkan Porsi\n";
        cout << "5. Urutkan Nomor Resi\n";
        cout << "6. Cari Nomor Resi\n";
        cout << "7. Cari Berdasarkan Jenjang\n";
        cout << "8. Simpan Laporan TXT\n";
        cout << "0. Keluar\n";

        // Validasi: jika bukan angka, tidak terjadi loop tak berujung
        pilih = bacaInt("Pilih Menu: ", 0, 8);

        switch (pilih) {
            case 1: tambahData(); break;
            case 2: tampilData(); break;
            case 3: urutMasalah(); break;
            case 4: urutPorsi(); break;
            case 5: urutResi(); break;
            case 6: cariResi(); break;
            case 7: cariJenjang(); break;
            case 8: simpanLaporan(); break;
            case 0: cout << "\nProgram selesai!\n"; break;
        }

    } while (pilih != 0);

    delete[] dataPenerima;
    dataPenerima = nullptr;

    return 0;
}
