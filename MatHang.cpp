#include "MatHang.h"
#include <iostream>

using namespace std;

MatHang::MatHang() : maSP(""), tenSP(""), giaNhap(0), giaBan(0) {}

MatHang::MatHang(string maSP, string tenSP, double giaNhap, double giaBan)
    : maSP(maSP), tenSP(tenSP), giaNhap(giaNhap), giaBan(giaBan) {}

MatHang::~MatHang() {}

// ----- Getter -----
string MatHang::getMaSP() const { return maSP; }
string MatHang::getTenSP() const { return tenSP; }
double MatHang::getGiaNhap() const { return giaNhap; }
double MatHang::getGiaBan() const { return giaBan; }

// ----- Setter -----
void MatHang::setMaSP(string maSP) { this->maSP = maSP; }
void MatHang::setTenSP(string tenSP) { this->tenSP = tenSP; }
void MatHang::setGiaNhap(double giaNhap) { this->giaNhap = giaNhap; }
void MatHang::setGiaBan(double giaBan) { this->giaBan = giaBan; }

void MatHang::hienThiThongTin() {
    cout << "Ma SP   : " << maSP << "\n";
    cout << "Ten SP  : " << tenSP << "\n";
    cout << "Gia Nhap: " << giaNhap << "\n";
    cout << "Gia Ban : " << giaBan << "\n";
}