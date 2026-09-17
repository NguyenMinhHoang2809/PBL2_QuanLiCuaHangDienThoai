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
    std::string maNhanVien;
    std::string ngayLap;
    double tongTien;
    double doanhThu; // = tong (Gia Ban - Gia Nhap) cua tung san pham trong hoa don

public:
    HoaDon();
    HoaDon(std::string maHoaDon, std::string maKhachHang, std::string maNhanVien,
           std::string ngayLap, double tongTien, double doanhThu);

    // ----- Getter -----
    std::string getMaHoaDon() const;
    std::string getMaKhachHang() const;
    std::string getMaNhanVien() const;
    std::string getNgayLap() const;
    double getTongTien() const;
    double getDoanhThu() const;

    // ----- Setter -----
    void setMaHoaDon(std::string maHoaDon);
    void setMaKhachHang(std::string maKhachHang);
    void setMaNhanVien(std::string maNhanVien);
    void setNgayLap(std::string ngayLap);
    void setTongTien(double tongTien);
    void setDoanhThu(double doanhThu);

    // Tao thu muc DataHoaDon (neu chua co) va xuat thong tin hoa don
    // ra file: DataHoaDon/HD_[maHoaDon].txt
    void luuFile();

    // Doc noi dung file hoa don (dua theo maHoaDon hien tai) va in len Console
    void inHoaDon();
};

#endif // HOADON_H