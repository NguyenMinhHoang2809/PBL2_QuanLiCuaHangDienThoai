#include "DienThoai.h"
#include <iostream>

using namespace std;

DienThoai::DienThoai() : MatHang() {}

DienThoai::DienThoai(string maSP, string tenSP, double giaNhap, double giaBan)
    : MatHang(maSP, tenSP, giaNhap, giaBan) {}

DienThoai::~DienThoai() {}

void DienThoai::hienThiThongTin() {
    // Goi lai ham cua lop cha de tai su dung code (khong lap lai logic in
    // maSP/tenSP/giaNhap/giaBan), sau do bo sung them loai san pham.
    MatHang::hienThiThongTin();
    cout << "Loai SP : Dien Thoai\n";
}
