#ifndef TABLET_H
#define TABLET_H

#include "MatHang.h"

// ============================================================
// Lop Tablet - ke thua MatHang, bo sung thuoc tinh kichThuocManHinh
// ============================================================
class Tablet : public MatHang {
private:
    double kichThuocManHinh; // don vi: inch

public:
    Tablet();
    Tablet(std::string maSP, std::string tenSP, double giaNhap, double giaBan,
           double kichThuocManHinh);
    ~Tablet() override;

    // ----- Getter / Setter rieng cua Tablet -----
    double getKichThuocManHinh() const;
    void setKichThuocManHinh(double kichThuocManHinh);

    // Ghi de ham ao cua lop co so MatHang
    void hienThiThongTin() override;
};

#endif // TABLET_H