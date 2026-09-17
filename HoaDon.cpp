#include "HoaDon.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>

using namespace std;
namespace fs = std::filesystem;

HoaDon::HoaDon() : maHoaDon(""), maKhachHang(""), maNhanVien(""), ngayLap(""), tongTien(0), doanhThu(0) {}

HoaDon::HoaDon(string maHoaDon, string maKhachHang, string maNhanVien, string ngayLap,
               double tongTien, double doanhThu)
    : maHoaDon(maHoaDon), maKhachHang(maKhachHang), maNhanVien(maNhanVien),
      ngayLap(ngayLap), tongTien(tongTien), doanhThu(doanhThu) {}

// ----- Getter -----
string HoaDon::getMaHoaDon() const { return maHoaDon; }
string HoaDon::getMaKhachHang() const { return maKhachHang; }
string HoaDon::getMaNhanVien() const { return maNhanVien; }
string HoaDon::getNgayLap() const { return ngayLap; }
double HoaDon::getTongTien() const { return tongTien; }
double HoaDon::getDoanhThu() const { return doanhThu; }

// ----- Setter -----
void HoaDon::setMaHoaDon(string maHoaDon) { this->maHoaDon = maHoaDon; }
void HoaDon::setMaKhachHang(string maKhachHang) { this->maKhachHang = maKhachHang; }
void HoaDon::setMaNhanVien(string maNhanVien) { this->maNhanVien = maNhanVien; }
void HoaDon::setNgayLap(string ngayLap) { this->ngayLap = ngayLap; }
void HoaDon::setTongTien(double tongTien) { this->tongTien = tongTien; }
void HoaDon::setDoanhThu(double doanhThu) { this->doanhThu = doanhThu; }

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
    file << "Ma Nhan Vien: " << maNhanVien << "\n";
    file << "Ngay Lap: " << ngayLap << "\n";
    // fixed + setprecision(0): tien VND la so nguyen, tranh cout in dang
    // khoa hoc (vd: 1.55e+07) khi tongTien la so lon (>= 1 trieu).
    file << fixed << setprecision(0);
    file << "Tong Tien: " << tongTien << "\n";
    // Doanh Thu = tong (Gia Ban - Gia Nhap) cua tung san pham trong hoa
    // don nay - duoc TINH SAN va LUU LAI ngay tai thoi diem lap hoa don
    // (khong tinh lai sau), vi Gia Nhap cua san pham co the bi Quan Ly
    // sua doi sau nay, se lam sai lech doanh thu cua cac hoa don cu neu
    // tinh lai luc xem bao cao.
    file << "Doanh Thu: " << doanhThu << "\n";
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