#ifndef DIENTHOAI_H
#define DIENTHOAI_H

#include "MatHang.h"

// ============================================================
// Lop DienThoai - ke thua MatHang (dung toan bo thuoc tinh cua lop cha:
// maSP, tenSP, giaNhap, giaBan) va ghi de ham ao hienThiThongTin().
// ============================================================
class DienThoai : public MatHang {
public:
    DienThoai();
    DienThoai(std::string maSP, std::string tenSP, double giaNhap, double giaBan);
    ~DienThoai() override;

    // Ghi de ham ao cua lop co so MatHang
    void hienThiThongTin() override;
};

#endif // DIENTHOAI_H
