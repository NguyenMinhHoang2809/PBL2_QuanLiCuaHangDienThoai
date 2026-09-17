#ifndef NHANVIENBANHANG_H
#define NHANVIENBANHANG_H

#include "Nguoi.h"

// ============================================================
// Lop NhanVienBanHang - ke thua Nguoi
// Quyen han: Tao tai khoan Khach hang, Lap hoa don ban hang
// ============================================================
class NhanVienBanHang : public Nguoi {
public:
    NhanVienBanHang();
    NhanVienBanHang(std::string id, std::string mk);
    ~NhanVienBanHang() override;

    void hienThiMenu() override;
};

#endif // NHANVIENBANHANG_H