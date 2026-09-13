#include "Account.h"
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

// ------------------------------------------------------------
// TaoAccount: giu nguyen logic tham chieu, chi bo sung namespace fs
// ------------------------------------------------------------
void TaoAccount(string vaiTroNguoiTao) {
    string ID, MatKhau, ThuMuc;
    cout << "Nhap ID: "; cin >> ID;

    if (ID.substr(0, 3) == "KH_") ThuMuc = "DataKhachHang";
    else if (ID.substr(0, 3) == "NV_") ThuMuc = "DataNhanVien";
    else if (ID.substr(0, 3) == "QL_") ThuMuc = "DataQuanLi";
    else { cout << "ID khong hop le!" << endl; return; }

    // Rang buoc quyen tao tai khoan
    if (vaiTroNguoiTao == "NV" && ThuMuc != "DataKhachHang") {
        cout << "Nhan vien chi duoc tao tai khoan Khach hang!" << endl; return;
    }
    if (vaiTroNguoiTao == "KH") {
        cout << "Khach hang khong co quyen tao tai khoan!" << endl; return;
    }

    fs::create_directories(ThuMuc);
    string TenFile = ThuMuc + "/" + ID + ".txt";

    if (fs::exists(TenFile)) {
        cout << "ID nay da ton tai!" << endl; return;
    }

    cout << "Mat Khau: "; cin >> MatKhau;
    ofstream file(TenFile);
    file << ID << "\n" << MatKhau << "\n";
    file.close();
    cout << "Da tao account thanh cong!\n";
}

// ------------------------------------------------------------
// DangNhap: doc file tuong ung theo tien to ID, doi chieu mat khau.
// Dong 1 cua file = ID, dong 2 = Mat khau (dung format ghi cua TaoAccount)
// ------------------------------------------------------------
string DangNhap(string& idOut) {
    string ID, MatKhau, ThuMuc, vaiTro;
    cout << "ID: "; cin >> ID;
    cout << "Mat khau: "; cin >> MatKhau;

    if (ID.substr(0, 3) == "KH_") { ThuMuc = "DataKhachHang"; vaiTro = "KH"; }
    else if (ID.substr(0, 3) == "NV_") { ThuMuc = "DataNhanVien"; vaiTro = "NV"; }
    else if (ID.substr(0, 3) == "QL_") { ThuMuc = "DataQuanLi"; vaiTro = "QL"; }
    else { cout << "ID khong hop le!" << endl; return ""; }

    string TenFile = ThuMuc + "/" + ID + ".txt";
    if (!fs::exists(TenFile)) {
        cout << "Tai khoan khong ton tai!" << endl; return "";
    }

    ifstream file(TenFile);
    string idTrongFile, mkTrongFile;
    getline(file, idTrongFile);
    getline(file, mkTrongFile);
    file.close();

    if (mkTrongFile != MatKhau) {
        cout << "Sai mat khau!" << endl; return "";
    }

    idOut = ID;
    return vaiTro;
}