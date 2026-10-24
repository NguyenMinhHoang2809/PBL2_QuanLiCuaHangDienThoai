#ifndef KHACHHANG_H
#define KHACHHANG_H

#include "Nguoi.h"

// ============================================================
// Lop KhachHang - ke thua Nguoi
// Quyen han: Xem lich su mua hang (Stack LIFO)
// ============================================================
class KhachHang : public Nguoi {
public:
    KhachHang();
    KhachHang(std::string id, std::string mk, std::string hoTen = "");
    ~KhachHang() override;

    void hienThiMenu() override;
};

#endif // KHACHHANG_H
