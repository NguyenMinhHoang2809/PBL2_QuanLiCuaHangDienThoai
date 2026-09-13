#ifndef DIENTHOAI_H
#define DIENTHOAI_H

#include "MatHang.h"

// ============================================================
// Lop DienThoai - ke thua MatHang, bo sung thuoc tinh cauHinh
// ============================================================
class DienThoai : public MatHang {
private:
    std::string cauHinh;

public:
    DienThoai();
    DienThoai(std::string maSP, std::string tenSP, double giaNhap, double giaBan,
               std::string cauHinh);
    ~DienThoai() override;

    // ----- Getter / Setter rieng cua DienThoai -----
    std::string getCauHinh() const;
    void setCauHinh(std::string cauHinh);

    // Ghi de ham ao cua lop co so MatHang
    void hienThiThongTin() override;
};

#endif // DIENTHOAI_H