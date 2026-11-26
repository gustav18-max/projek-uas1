#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
using namespace std;

// ==========================================
// 1. ARRAY STATIS & KONSTANTA
// ==========================================
const int JUM_GIZI = 5;
const float SKOR_AMAN = 90.0;
string namaGizi[JUM_GIZI] = {"Karbohidrat", "Protein", "Lemak", "Sayuran", "Buah"};

// ==========================================
// 2. ENUM
// ==========================================
enum Jenjang { SD = 1, SMP, SMA, POSYANDU };
string namaJenjang[5] = {"", "SD", "SMP", "SMA", "Posyandu"};

// Array 2D: Standar minimal gizi tiap jenjang {Karbo, Protein, Lemak, Sayur, Buah}
float standarGizi[5][JUM_GIZI] = {
    {0, 0, 0, 0, 0},          // index 0 tidak dipakai
    {80, 50, 20, 75, 100},    // 1. SD
    {100, 65, 25, 100, 120},  // 2. SMP
    {120, 80, 30, 120, 150},  // 3. SMA
    {60, 45, 18, 60, 80}      // 4. Posyandu
};

// Array 2D: Batas wajar biaya bahan baku per porsi {minimal, maksimal}
float batasBiaya[5][2] = {
    {0, 0},            // index 0 tidak dipakai
    {7000, 8500},      // 1. SD
    {9000, 10500},     // 2. SMP
    {9000, 10500},     // 3. SMA
    {7000, 8500}       // 4. Posyandu
};

// ==========================================
// 3. UNION
// ==========================================
union Identitas {
    int npsn;
    int kodePosyandu;
};

// ==========================================
// 4. STRUCT
// ==========================================
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
    string keterangan;
};

// ==========================================
// 5. ARRAY DINAMIS + POINTER
// ==========================================
Penerima* dataPenerima = nullptr;
int jumlah = 0;

// ==========================================
// FUNGSI BANTU INPUT (VALIDASI)
// ==========================================
int bacaInt(string pesan, int minVal, int maxVal) {
    int x;
    while (true) {
        cout << pesan;
        if (cin >> x && x >= minVal && x <= maxVal) {
            cin.ignore(1000, '\n');
            return x;
        }
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Input tidak valid!\n";
    }
}

float bacaFloat(string pesan, float minVal) {
    float x;
    while (true) {
        cout << pesan;
        if (cin >> x && x >= minVal) {
            cin.ignore(1000, '\n');
            return x;
        }
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Input tidak valid!\n";
    }
}

string bacaTeks(string pesan) {
    string s;
    do {
        cout << pesan;
        getline(cin, s);
        if (s == "") cout << "Tidak boleh kosong!\n";
    } while (s == "");
    return s;
}

// Fungsi bantu tukar 2 data struct (dipakai di Sorting)
void tukarData(Penerima &a, Penerima &b) {
    Penerima temp = a;
    a = b;
    b = temp;
}

// ==========================================
// FUNGSI LOGIKA PERHITUNGAN & EVALUASI
// ==========================================
float hitungGizi(float g[], Jenjang j) {
    float total = 0;
    for (int i = 0; i < JUM_GIZI; i++) {
        float stdMin = standarGizi[j][i];
        float rasio = g[i] / stdMin;
        float nilai = 0;

        if (rasio < 1.0) {
            nilai = rasio * 100.0;
        } else if (rasio <= 1.8) {
            nilai = 100.0;
        } else {
            float kelebihan = rasio - 1.8;
            nilai = 100.0 - (kelebihan * 20.0);
            if (nilai < 0) nilai = 0;
        }
        total += nilai;
    }
    return total / JUM_GIZI;
}

void evaluasiPaket(Penerima &p) {
    bool adaKomponenKurang = false;
    bool adaKomponenBerlebih = false;

    for (int i = 0; i < JUM_GIZI; i++) {
        float stdMin = standarGizi[p.jenjang][i];
        if (p.gramasi[i] < stdMin) adaKomponenKurang = true;
        if (p.gramasi[i] > stdMin * 1.8) adaKomponenBerlebih = true;
    }

    bool giziBermasalah = (p.skor < SKOR_AMAN) || adaKomponenBerlebih;
    bool budgetKurang = (p.biaya < batasBiaya[p.jenjang][0]);
    bool budgetBerlebih = (p.biaya > batasBiaya[p.jenjang][1]);

    p.masalah = (giziBermasalah || budgetKurang || budgetBerlebih);

    if (!p.masalah) {
        p.keterangan = "Normal";
    } else {
        string ket = "";
        if (p.skor < SKOR_AMAN && adaKomponenKurang) {
            ket += "Gizi Kurang";
        }
        if (adaKomponenBerlebih) {
            if (ket != "") ket += ", ";
            ket += "Gizi Berlebih";
        }
        if (budgetKurang) {
            if (ket != "") ket += ", ";
            ket += "Budget Kurang";
        }
        if (budgetBerlebih) {
            if (ket != "") ket += ", ";
            ket += "Budget Berlebih";
        }
        p.keterangan = ket;
    }
}

bool resiSudahAda(string resi) {
    for (int i = 0; i < jumlah; i++) {
        if (dataPenerima[i].resi == resi) return true;
    }
    return false;
}

// ==========================================
// FUNGSI CETAK TABEL DI LAYAR
// ==========================================
void cetakGaris() {
    for (int i = 0; i < 128; i++) cout << "-";
    cout << "\n";
}

void cetakHeader() {
    cetakGaris();
    cout << left << setw(4) << "No" << setw(10) << "Resi" << setw(18) << "Nama"
         << setw(10) << "Jenjang" << setw(11) << "NPSN/Kode" << setw(7) << "Porsi"
         << setw(11) << "Biaya(Rp)" << setw(9) << "Skor" << setw(17) << "Status"
         << "Keterangan\n";
    cetakGaris();
}

void cetakBaris(Penerima p, int no) {
    int id = (p.jenjang == POSYANDU) ? p.id.kodePosyandu : p.id.npsn;
    cout << left << setw(4) << no << setw(10) << p.resi.substr(0, 9)
         << setw(18) << p.nama.substr(0, 17) << setw(10) << namaJenjang[p.jenjang]
         << setw(11) << id << setw(7) << p.porsi
         << fixed << setprecision(0) << setw(11) << p.biaya
         << setprecision(2) << right << setw(6) << p.skor << "%  " << left
         << setw(17) << (p.masalah ? "PERLU DIPERIKSA" : "SESUAI STANDAR")
         << p.keterangan << "\n";
}

// ==========================================
// MENU 1: TAMBAH DATA (ARRAY DINAMIS)
// ==========================================
void tambahData() {
    Penerima baru;
    cout << "\n=== INPUT DATA MBG ===\n";

    baru.resi = bacaTeks("Nomor Resi: ");
    while (resiSudahAda(baru.resi)) {
        cout << "Nomor resi sudah terdaftar!\n";
        baru.resi = bacaTeks("Nomor Resi: ");
    }

    baru.nama = bacaTeks("Nama Sekolah/Instansi: ");

    cout << "\nPilih Jenjang:\n1. SD\n2. SMP\n3. SMA\n4. Posyandu\n";
    baru.jenjang = (Jenjang)bacaInt("Pilihan: ", 1, 4);

    if (baru.jenjang == POSYANDU)
        baru.id.kodePosyandu = bacaInt("Kode Posyandu: ", 0, 999999999);
    else
        baru.id.npsn = bacaInt("NPSN: ", 0, 999999999);

    baru.porsi = bacaInt("Jumlah Porsi: ", 1, 999999999);
    baru.biaya = bacaFloat("Biaya per Porsi: Rp", 0);

    cout << "\n=== INPUT GRAMASI AKTUAL ===\n";
    for (int i = 0; i < JUM_GIZI; i++) {
        baru.gramasi[i] = bacaFloat(namaGizi[i] + " (gram): ", 0);
    }

    baru.skor = hitungGizi(baru.gramasi, baru.jenjang);
    evaluasiPaket(baru);

    // Perbesar ukuran array dinamis
    Penerima* temp = new Penerima[jumlah + 1];
    for (int i = 0; i < jumlah; i++) {
        temp[i] = dataPenerima[i];
    }
    temp[jumlah] = baru;
    jumlah++;

    delete[] dataPenerima;
    dataPenerima = temp;

    cout << "\nData berhasil ditambahkan!\n";
    cout << "Skor Gizi  : " << fixed << setprecision(2) << baru.skor << "%\n";
    cout << "Status     : " << (baru.masalah ? "PERLU DIPERIKSA" : "SESUAI STANDAR") << "\n";
    cout << "Keterangan : " << baru.keterangan << "\n";
}

// ==========================================
// MENU 2: TAMPILKAN DATA
// ==========================================
void tampilData() {
    if (jumlah == 0) {
        cout << "\nBelum ada data!\n";
        return;
    }
    cout << "\n=== DATA PENERIMA MBG ===\n";
    cetakHeader();
    for (int i = 0; i < jumlah; i++) {
        cetakBaris(dataPenerima[i], i + 1);
    }
    cetakGaris();
}

// ==========================================
// MENU 3: INSERTION SORT (PRIORITAS MASALAH)
// ==========================================
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

// ==========================================
// MENU 4: SELECTION SORT (PORSI TERBANYAK)
// ==========================================
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
        tukarData(dataPenerima[i], dataPenerima[terbesar]);
    }
    cout << "\nData berhasil diurutkan berdasarkan porsi!\n";
    tampilData();
}

// ==========================================
// BUBBLE SORT (URUT RESI)
// ==========================================
void sortResi() {
    for (int i = 0; i < jumlah - 1; i++) {
        bool tukar = false;
        for (int j = 0; j < jumlah - i - 1; j++) {
            if (dataPenerima[j].resi > dataPenerima[j + 1].resi) {
                tukarData(dataPenerima[j], dataPenerima[j + 1]);
                tukar = true;
            }
        }
        if (!tukar) break;
    }
}

// MENU 5: URUTKAN NOMOR RESI
void urutResi() {
    if (jumlah == 0) {
        cout << "\nData masih kosong!\n";
        return;
    }
    sortResi();
    cout << "\nNomor resi berhasil diurutkan!\n";
    tampilData();
}

// ==========================================
// MENU 6: BINARY SEARCH (CARI RESI)
// ==========================================
void cariResi() {
    if (jumlah == 0) {
        cout << "\nData masih kosong!\n";
        return;
    }

    sortResi(); // Syarat binary search: data harus terurut
    string cari = bacaTeks("\nMasukkan nomor resi: ");

    int kiri = 0, kanan = jumlah - 1;
    while (kiri <= kanan) {
        int tengah = kiri + (kanan - kiri) / 2;

        if (dataPenerima[tengah].resi == cari) {
            Penerima p = dataPenerima[tengah];
            cout << "\nData ditemukan!\n"
                 << "Nama       : " << p.nama << "\n"
                 << "Porsi      : " << p.porsi << "\n"
                 << "Biaya      : Rp" << fixed << setprecision(0) << p.biaya << "\n"
                 << "Skor Gizi  : " << setprecision(2) << p.skor << "%\n"
                 << "Status     : " << (p.masalah ? "PERLU DIPERIKSA" : "SESUAI STANDAR") << "\n"
                 << "Keterangan : " << p.keterangan << "\n";
            return;
        }
        if (dataPenerima[tengah].resi < cari) kiri = tengah + 1;
        else                                  kanan = tengah - 1;
    }
    cout << "\nNomor resi tidak ditemukan!\n";
}

// ==========================================
// MENU 7: LINEAR SEARCH (CARI JENJANG)
// ==========================================
void cariJenjang() {
    if (jumlah == 0) {
        cout << "\nData masih kosong!\n";
        return;
    }

    cout << "\n1. SD\n2. SMP\n3. SMA\n4. Posyandu\n";
    Jenjang pilih = (Jenjang)bacaInt("Pilih jenjang: ", 1, 4);

    int no = 0;
    cetakHeader();
    for (int i = 0; i < jumlah; i++) {
        if (dataPenerima[i].jenjang == pilih) {
            no++;
            cetakBaris(dataPenerima[i], no);
        }
    }
    cetakGaris();
    if (no == 0) cout << "Data tidak ditemukan!\n";
}

// ==========================================
// MENU 8: FILE I/O (SIMPAN LAPORAN TXT)
// ==========================================
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
    for (int i = 0; i < 128; i++) file << "-";
    file << "\n";
    file << left << setw(4) << "No" << setw(10) << "Resi" << setw(18) << "Nama"
         << setw(10) << "Jenjang" << setw(11) << "NPSN/Kode" << setw(7) << "Porsi"
         << setw(11) << "Biaya(Rp)" << setw(9) << "Skor" << setw(17) << "Status"
         << "Keterangan\n";
    for (int i = 0; i < 128; i++) file << "-";
    file << "\n";

    for (int i = 0; i < jumlah; i++) {
        Penerima p = dataPenerima[i];
        int id = (p.jenjang == POSYANDU) ? p.id.kodePosyandu : p.id.npsn;
        file << left << setw(4) << (i + 1) << setw(10) << p.resi.substr(0, 9)
             << setw(18) << p.nama.substr(0, 17) << setw(10) << namaJenjang[p.jenjang]
             << setw(11) << id << setw(7) << p.porsi
             << fixed << setprecision(0) << setw(11) << p.biaya
             << setprecision(2) << right << setw(6) << p.skor << "%  " << left
             << setw(17) << (p.masalah ? "PERLU DIPERIKSA" : "SESUAI STANDAR")
             << p.keterangan << "\n";
    }

    for (int i = 0; i < 128; i++) file << "-";
    file << "\n";
    file.close();

    cout << "\nLaporan berhasil disimpan ke Laporan_MBG.txt!\n";
}

// ==========================================
// MENU 9: HAPUS DATA (ARRAY DINAMIS)
// ==========================================
void hapusData() {
    if (jumlah == 0) {
        cout << "\nData masih kosong, tidak ada yang dihapus!\n";
        return;
    }

    tampilData();
    string target = bacaTeks("\nMasukkan nomor resi yang akan dihapus: ");

    int idxHapus = -1;
    for (int i = 0; i < jumlah; i++) {
        if (dataPenerima[i].resi == target) {
            idxHapus = i;
            break;
        }
    }

    if (idxHapus == -1) {
        cout << "\nNomor resi tidak ditemukan!\n";
        return;
    }

    cout << "\nData yang akan dihapus: " << dataPenerima[idxHapus].nama
         << " (" << dataPenerima[idxHapus].resi << ")\n";
    string yakin = bacaTeks("Yakin ingin menghapus data ini? (y/n): ");

    if (yakin != "y" && yakin != "Y") {
        cout << "Penghapusan dibatalkan.\n";
        return;
    }

    if (jumlah == 1) {
        delete[] dataPenerima;
        dataPenerima = nullptr;
        jumlah = 0;
    } else {
        Penerima* temp = new Penerima[jumlah - 1];
        int idxBaru = 0;
        for (int i = 0; i < jumlah; i++) {
            if (i != idxHapus) {
                temp[idxBaru] = dataPenerima[i];
                idxBaru++;
            }
        }
        delete[] dataPenerima;
        dataPenerima = temp;
        jumlah--;
    }

    cout << "\nData berhasil dihapus!\n";
}

// ==========================================
// PROGRAM UTAMA
// ==========================================
int main() {
    int pilih;
    do {
        cout << "\n================================\n"
             << "   SISTEM MONITORING MBG\n"
             << "================================\n"
             << "1. Tambah Data Penerima\n"
             << "2. Tampilkan Data\n"
             << "3. Prioritas Paket Bermasalah\n"
             << "4. Urutkan Berdasarkan Porsi\n"
             << "5. Urutkan Nomor Resi\n"
             << "6. Cari Nomor Resi\n"
             << "7. Cari Berdasarkan Jenjang\n"
             << "8. Simpan Laporan TXT\n"
             << "9. Hapus Data Penerima\n"
             << "0. Keluar\n";

        pilih = bacaInt("Pilih Menu: ", 0, 9);

        switch (pilih) {
            case 1: tambahData();    break;
            case 2: tampilData();    break;
            case 3: urutMasalah();   break;
            case 4: urutPorsi();     break;
            case 5: urutResi();      break;
            case 6: cariResi();      break;
            case 7: cariJenjang();   break;
            case 8: simpanLaporan(); break;
            case 9: hapusData();     break;
            case 0: cout << "\nProgram selesai!\n"; break;
        }
    } while (pilih != 0);

    delete[] dataPenerima;
    return 0;
}
