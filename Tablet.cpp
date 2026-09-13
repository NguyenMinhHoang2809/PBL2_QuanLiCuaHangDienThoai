#include "Tablet.h"
#include <iostream>

using namespace std;

Tablet::Tablet() : MatHang(), kichThuocManHinh(0) {}

Tablet::Tablet(string maSP, string tenSP, double giaNhap, double giaBan, double kichThuocManHinh)
    : MatHang(maSP, tenSP, giaNhap, giaBan), kichThuocManHinh(kichThuocManHinh) {}

Tablet::~Tablet() {}

double Tablet::getKichThuocManHinh() const { return kichThuocManHinh; }
void Tablet::setKichThuocManHinh(double kichThuocManHinh) {
    this->kichThuocManHinh = kichThuocManHinh;
}

void Tablet::hienThiThongTin() {
    // Goi lai ham cua lop cha de tai su dung code, sau do bo sung
    // thong tin rieng cua Tablet.
    MatHang::hienThiThongTin();
    cout << "Loai SP           : Tablet\n";
    cout << "Kich Thuoc Man Hinh: " << kichThuocManHinh << " inch\n";
}