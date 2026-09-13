#ifndef HOADON_H
#define HOADON_H

#include <string>

// ============================================================
// Lop HoaDon - dai dien cho mot hoa don ban hang.
// Duoc luu ra file: DataHoaDon/HD_[maHoaDon].txt
// ============================================================
class HoaDon {
private:
    std::string maHoaDon;
    std::string maKhachHang;
    std::string ngayLap;
    double tongTien;

public:
    HoaDon();
    HoaDon(std::string maHoaDon, std::string maKhachHang, std::string ngayLap, double tongTien);

    // ----- Getter -----
    std::string getMaHoaDon() const;
    std::string getMaKhachHang() const;
    std::string getNgayLap() const;
    double getTongTien() const;

    // ----- Setter -----
    void setMaHoaDon(std::string maHoaDon);
    void setMaKhachHang(std::string maKhachHang);
    void setNgayLap(std::string ngayLap);
    void setTongTien(double tongTien);

    // Tao thu muc DataHoaDon (neu chua co) va xuat thong tin hoa don
    // ra file: DataHoaDon/HD_[maHoaDon].txt
    void luuFile();

    // Doc noi dung file hoa don (dua theo maHoaDon hien tai) va in len Console
    void inHoaDon();
};

#endif // HOADON_H