#ifndef CHUCUAHANG_H
#define CHUCUAHANG_H

#include "Nguoi.h"

// ============================================================
// Lop ChuCuaHang (Quan Ly) - ke thua Nguoi
// Quyen han: Nhap hang hoa moi, Tao tai khoan Quan ly / Nhan vien,
//            Tim kiem & quan ly Nhan vien, Xem bao cao doanh thu
// ============================================================
class ChuCuaHang : public Nguoi {
public:
    ChuCuaHang();
    ChuCuaHang(std::string id, std::string mk, std::string hoTen = "");
    ~ChuCuaHang() override;

    // Hien thuc lai ham thuan ao cua lop co so Nguoi
    void hienThiMenu() override;
};

#endif // CHUCUAHANG_H
