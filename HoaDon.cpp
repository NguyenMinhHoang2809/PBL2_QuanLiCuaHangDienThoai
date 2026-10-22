#include "HoaDon.h"
#include "TienIch.h"
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
// hoa don ra file DataHoaDon/HD_[maHoaDon].txt
// ------------------------------------------------------------
void HoaDon::luuFile() {
    string thuMuc = "DataHoaDon";
    fs::create_directories(thuMuc);

    string tenFile = thuMuc + "/HD_" + maHoaDon + ".txt";

    ofstream file(tenFile);
    if (!file.is_open()) {
        cout << "\n";
        inThongBaoForm("Không thể tạo file hóa đơn!", LOAI_LOI);
        return;
    }

    file << "Ma Hoa Don: HD_" << maHoaDon << "\n";
    file << "Ma Khach Hang: " << maKhachHang << "\n";
    file << "Ma Nhan Vien: " << maNhanVien << "\n";
    file << "Ngay Lap: " << ngayLap << "\n";
    file << fixed << setprecision(0);
    file << "Tong Tien: " << tongTien << "\n";
    file << "Doanh Thu: " << doanhThu << "\n";
    file.close();

    cout << "\n";
    inThongBaoForm("Đã lưu hóa đơn thành công: " + tenFile, LOAI_THANH_CONG);
}

// ------------------------------------------------------------
// inHoaDon(): doc lai chinh file DataHoaDon/HD_[maHoaDon].txt va in
// noi dung trong khung phang can giua man hinh.
// ------------------------------------------------------------
void HoaDon::inHoaDon() {
    string tenFile = "DataHoaDon/HD_" + maHoaDon + ".txt";

    ifstream file(tenFile);
    if (!file.is_open()) {
        inThongBaoForm("Không tìm thấy hóa đơn " + tenFile + "!", LOAI_LOI);
        return;
    }

    veTieuDeForm("NỘI DUNG HÓA ĐƠN", "", 58);
    string dong;
    while (getline(file, dong)) {
        inDongTrongKhungForm(dong, 58);
    }
    file.close();
}
