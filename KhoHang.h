#ifndef KHOHANG_H
#define KHOHANG_H

#include <string>
#include <vector>

// ============================================================
// Module KHO HANG - doc va trinh bay toan bo du lieu trong
// DataMatHang/<TenMatHang>/<SoSeri>.txt
// Dung chung boi ChuCuaHang (xem duoc Gia Nhap) va
// NhanVienBanHang (KHONG xem duoc Gia Nhap).
// ============================================================

// Mot ban ghi mat hang doc tu file, phuc vu sap xep/loc trong bo nho
// (thay vi doc di doc lai o dia nhieu lan).
struct ThongTinMatHang {
    std::string tenMatHang;
    std::string soSeri;
    double giaNhap;
    double giaBan;
    bool daBan;
    std::string maKhachHangDaMua;
};

// Doc toan bo mat hang trong DataMatHang (duyet de quy moi thu muc con).
std::vector<ThongTinMatHang> docToanBoKhoHang();

// Xuat bang danh sach kho hang, co menu chon kieu sap xep.
// hienGiaNhap = true  -> danh cho Quan Ly (thay ca Gia Nhap va Gia Ban)
// hienGiaNhap = false -> danh cho Nhan Vien (CHI thay Gia Ban)
void XuatDanhSachKhoHang(bool hienGiaNhap);

// Tu van ban hang: nhap ngan sach cua khach, goi y cac may phu hop.
// Chi xet cac may CHUA BAN va co Gia Ban <= ngan sach.
void TuVanBanHang();

#endif // KHOHANG_H