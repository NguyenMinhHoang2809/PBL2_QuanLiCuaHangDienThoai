#ifndef MATHANG_H
#define MATHANG_H

#include <string>

// ============================================================
// Lop co so MatHang - dai dien cho mot mat hang bat ky trong kho.
// Cac lop dan xuat: DienThoai, Tablet
// ============================================================
class MatHang {
protected:
    std::string maSP;
    std::string tenSP;
    double giaNhap;
    double giaBan;

public:
    MatHang();
    MatHang(std::string maSP, std::string tenSP, double giaNhap, double giaBan);
    virtual ~MatHang();

    // ----- Getter -----
    std::string getMaSP() const;
    std::string getTenSP() const;
    double getGiaNhap() const;
    double getGiaBan() const;

    // ----- Setter -----
    void setMaSP(std::string maSP);
    void setTenSP(std::string tenSP);
    void setGiaNhap(double giaNhap);
    void setGiaBan(double giaBan);

    // Ham ao - cac lop con co the (nhung khong bat buoc) ghi de lai
    // de bo sung thong tin rieng (cau hinh, kich thuoc man hinh...)
    virtual void hienThiThongTin();
};

#endif // MATHANG_H