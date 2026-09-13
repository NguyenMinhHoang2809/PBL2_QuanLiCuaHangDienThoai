#ifndef KHACHHANG_H
#define KHACHHANG_H

#include "Nguoi.h"

// ============================================================
// Lop KhachHang - ke thua Nguoi
// Quyen han: Mua hang, Xem lich su mua hang (Stack LIFO)
// ============================================================
class KhachHang : public Nguoi {
public:
    KhachHang();
    KhachHang(std::string id, std::string mk);
    ~KhachHang() override;

    void hienThiMenu() override;
};

#endif // KHACHHANG_H