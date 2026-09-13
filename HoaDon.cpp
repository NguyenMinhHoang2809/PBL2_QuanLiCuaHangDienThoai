#include "HoaDon.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>

using namespace std;
namespace fs = std::filesystem;

HoaDon::HoaDon() : maHoaDon(""), maKhachHang(""), ngayLap(""), tongTien(0) {}

HoaDon::HoaDon(string maHoaDon, string maKhachHang, string ngayLap, double tongTien)
    : maHoaDon(maHoaDon), maKhachHang(maKhachHang), ngayLap(ngayLap), tongTien(tongTien) {}

// ----- Getter -----
string HoaDon::getMaHoaDon() const { return maHoaDon; }
string HoaDon::getMaKhachHang() const { return maKhachHang; }
string HoaDon::getNgayLap() const { return ngayLap; }
double HoaDon::getTongTien() const { return tongTien; }

// ----- Setter -----
void HoaDon::setMaHoaDon(string maHoaDon) { this->maHoaDon = maHoaDon; }
void HoaDon::setMaKhachHang(string maKhachHang) { this->maKhachHang = maKhachHang; }
void HoaDon::setNgayLap(string ngayLap) { this->ngayLap = ngayLap; }
void HoaDon::setTongTien(double tongTien) { this->tongTien = tongTien; }

// ------------------------------------------------------------
// luuFile(): tao thu muc DataHoaDon (neu chua co) va ghi thong tin
// hoa don ra file DataHoaDon/HD_[maHoaDon].txt theo dinh dang
// "Nhan: Gia Tri" tren moi dong - de sau nay de dang doc lai / parse
// (vd: KhachHang.cpp doc dong "Ma Khach Hang:" de loc hoa don).
// ------------------------------------------------------------
void HoaDon::luuFile() {
    string thuMuc = "DataHoaDon";
    fs::create_directories(thuMuc);

    string tenFile = thuMuc + "/HD_" + maHoaDon + ".txt";

    ofstream file(tenFile);
    if (!file.is_open()) {
        cout << "Khong the tao file hoa don!\n";
        return;
    }

    file << "Ma Hoa Don: HD_" << maHoaDon << "\n";
    file << "Ma Khach Hang: " << maKhachHang << "\n";
    file << "Ngay Lap: " << ngayLap << "\n";
    // fixed + setprecision(0): tien VND la so nguyen, tranh cout in dang
    // khoa hoc (vd: 1.55e+07) khi tongTien la so lon (>= 1 trieu).
    file << fixed << setprecision(0);
    file << "Tong Tien: " << tongTien << "\n";
    file.close();

    cout << "Da luu hoa don thanh cong: " << tenFile << "\n";
}

// ------------------------------------------------------------
// inHoaDon(): doc lai chinh file DataHoaDon/HD_[maHoaDon].txt (dua tren
// maHoaDon dang luu trong doi tuong) va in toan bo noi dung len Console.
// ------------------------------------------------------------
void HoaDon::inHoaDon() {
    string tenFile = "DataHoaDon/HD_" + maHoaDon + ".txt";

    ifstream file(tenFile);
    if (!file.is_open()) {
        cout << "Khong tim thay hoa don " << tenFile << "!\n";
        return;
    }

    cout << "\n----- NOI DUNG HOA DON -----\n";
    string dong;
    while (getline(file, dong)) {
        cout << dong << "\n";
    }
    cout << "-----------------------------\n";
    file.close();
}