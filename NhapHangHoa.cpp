#include "NhapHangHoa.h"
#include "Utils.h"      // nhapSoThucKhongAm()
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>

using namespace std;
namespace fs = std::filesystem;

// ------------------------------------------------------------
// chuanHoaTenThuMuc(): bo TOAN BO khoang trang trong ten mat hang de
// dung lam TEN THU MUC. Ly do: neu dung nguyen TenMatHang lam ten thu
// muc, hai lan nhap khac nhau ve cach go khoang trang cho CUNG MOT SAN
// PHAM (vd: "iPhone 15 Pro" va "iPhone15Pro") se bi he thong hieu la
// hai san pham khac nhau -> tao 2 thu muc rieng biet, du lieu bi phan
// tan. Chuan hoa ve dang khong dau cach truoc khi tao thu muc dam bao
// moi cach go (co/khong dau cach) deu quy ve DUNG MOT thu muc.
//
// Luu y: TenMatHang GOC (co dau cach, de doc) van duoc ghi nguyen ven
// vao noi dung file .txt ben trong - chi TEN THU MUC la bi chuan hoa.
// ------------------------------------------------------------
static string chuanHoaTenThuMuc(const string& ten) {
    string ketQua;
    for (char c : ten) {
        if (c != ' ') ketQua += c;
    }
    return ketQua;
}

// ------------------------------------------------------------
// TaoMatHang: giu nguyen logic tham chieu, nhung GiaNhap/GiaBan
// duoc doc bang nhapSoThucKhongAm() (kieu double, khong cho gia am)
// thay vi cin >> string nhu ban dau - tranh luu gia tri "rac"
// (chu, ky hieu...) vao file du lieu san pham.
// ------------------------------------------------------------
void TaoMatHang() {
    string TenMatHang, SoSeri;

    // Dung getline(cin >> ws, ...) thay vi cin >> de nhan duoc ten mat hang
    // co khoang trang (vd: "iPhone 15 Pro", "Samsung Galaxy Tab S9").
    // "cin >> ws" se bo qua het khoang trang/newline con sot lai trong buffer
    // truoc khi getline bat dau doc, tranh loi "troi lenh".
    cout << "Nhap Ten Mat Hang: "; getline(cin >> ws, TenMatHang);
    cout << "So seri: "; cin >> SoSeri;

    string ThuMucGoc = "DataMatHang";
    // Dung ten thu muc DA CHUAN HOA (khong khoang trang) - tranh tao
    // trung thu muc cho cung 1 san pham khi go khoang trang khac nhau.
    string ThuMucCon = ThuMucGoc + "/" + chuanHoaTenThuMuc(TenMatHang);
    string TenFile = ThuMucCon + "/" + SoSeri + ".txt";

    if (fs::exists(TenFile)) {
        cout << "Mat hang nay da ton tai!" << endl; return;
    }

    double GiaNhap = nhapSoThucKhongAm("Gia Nhap: ");
    double GiaBan = nhapSoThucKhongAm("Gia Ban: ");

    fs::create_directories(ThuMucCon);
    ofstream file(TenFile);
    // Ghi TenMatHang GOC (co dau cach) vao noi dung file de de doc,
    // du thu muc chua no da duoc chuan hoa bo dau cach.
    file << "Ten Mat Hang: " << TenMatHang << "\n";
    file << "So Seri: " << SoSeri << "\n";
    // fixed + setprecision(0): tranh in gia dang khoa hoc (vd: 2.5e+07)
    // khi gia tri >= 1 trieu, dong nhat voi cach lam o HoaDon::luuFile().
    file << fixed << setprecision(0);
    file << "Gia Nhap: " << GiaNhap << "\n";
    file << "Gia Ban: " << GiaBan << "\n";
    file.close();
    cout << "Da luu mat hang thanh cong!\n";
}