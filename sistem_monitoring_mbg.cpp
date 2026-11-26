#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <fstream>
using namespace std;

// 1. Konstanta & Array 2D Standar Gizi serta Batas Biaya
const int JML_JENJANG = 4;
const string NAMA_JENJANG[JML_JENJANG] = {"PAUD/TK", "SD", "SMP", "SMA/SMK"};

// Kolom: [0] Kalori Min (kkal), [1] Protein Min (gram), [2] Batas Biaya Maks (Rp)
const float STANDAR_GIZI[JML_JENJANG][3] = {
    {350.0f, 10.0f, 10000.0f}, // PAUD/TK
    {450.0f, 12.0f, 12000.0f}, // SD
    {600.0f, 15.0f, 15000.0f}, // SMP
    {700.0f, 18.0f, 15000.0f}  // SMA/SMK
};

// 2. Enum, Union, dan Struct Data Penerima
enum Jenjang { PAUD_TK = 0, SD = 1, SMP = 2, SMA_SMK = 3 };

union Identitas {
    int kodeSekolah;
    long long npsn;
};

struct Penerima {
    int noResi;
    Identitas id;
    string namaSekolah;
    Jenjang jenjang;
    int jumlahPorsi;
    float kalori;
    float protein;
    float biayaPerPorsi;
    float skorGizi;
    string status;
    string keterangan;
};

// 3. Pointer Array Dinamis
Penerima* dataPenerima = nullptr;
int jumlahData = 0;

// 4. Fungsi Bantu Validasi Input & Pertukaran Data
int bacaInt(const string& pesan, int minVal, int maxVal) {
    int nilai;
    while (true) {
        cout << pesan;
        if (cin >> nilai && nilai >= minVal && nilai <= maxVal) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return nilai;
        }
        cout << "Input tidak valid! Masukkan angka antara " << minVal << " - " << maxVal << ".\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

float bacaFloat(const string& pesan, float minVal, float maxVal) {
    float nilai;
    while (true) {
        cout << pesan;
        if (cin >> nilai && nilai >= minVal && nilai <= maxVal) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return nilai;
        }
        cout << "Input tidak valid! Masukkan angka antara " << minVal << " - " << maxVal << ".\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string bacaTeks(const string& pesan) {
    string teks;
    while (true) {
        cout << pesan;
        getline(cin, teks);
        if (!teks.empty()) return teks;
        cout << "Input tidak boleh kosong!\n";
    }
}

void tukarData(Penerima& a, Penerima& b) {
    Penerima temp = a;
    a = b;
    b = temp;
}

// Kerangka Menu Sementara (akan dilengkapi di tahap berikutnya)
int main() {
    int pilih;
    do {
        cout << "\n================================\n"
             << "   SISTEM MONITORING MBG\n"
             << "================================\n"
             << "1. Tambah Data Penerima\n"
             << "2. Tampilkan Data\n"
             << "0. Keluar\n";
        pilih = bacaInt("Pilih Menu: ", 0, 2);
        if (pilih == 0) cout << "\nProgram selesai!\n";
    } while (pilih != 0);

    delete[] dataPenerima;
    return 0;
}
