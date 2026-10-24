#ifndef NHANVIENBANHANG_H
#define NHANVIENBANHANG_H

#include "Nguoi.h"

// ============================================================
// Lop NhanVienBanHang - ke thua Nguoi
// Quyen han: Tao tai khoan Khach hang, Tim kiem thong tin Khach hang,
//            Lap hoa don ban hang, Xuat kho, Tu van ban hang
// ============================================================
class NhanVienBanHang : public Nguoi {
public:
    NhanVienBanHang();
    NhanVienBanHang(std::string id, std::string mk, std::string hoTen = "");
    ~NhanVienBanHang() override;

    void hienThiMenu() override;
};

#endif // NHANVIENBANHANG_H
