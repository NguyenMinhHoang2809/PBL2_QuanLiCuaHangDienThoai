#include "DienThoai.h"
#include <iostream>

using namespace std;

DienThoai::DienThoai() : MatHang(), cauHinh("") {}

DienThoai::DienThoai(string maSP, string tenSP, double giaNhap, double giaBan, string cauHinh)
    : MatHang(maSP, tenSP, giaNhap, giaBan), cauHinh(cauHinh) {}

DienThoai::~DienThoai() {}

string DienThoai::getCauHinh() const { return cauHinh; }
void DienThoai::setCauHinh(string cauHinh) { this->cauHinh = cauHinh; }

void DienThoai::hienThiThongTin() {
    // Goi lai ham cua lop cha de tai su dung code (khong lap lai logic in
    // maSP/tenSP/giaNhap/giaBan), sau do bo sung them thong tin rieng.
    MatHang::hienThiThongTin();
    cout << "Loai SP : Dien Thoai\n";
    cout << "Cau Hinh: " << cauHinh << "\n";
}