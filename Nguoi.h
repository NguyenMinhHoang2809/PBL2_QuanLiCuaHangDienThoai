#ifndef NGUOI_H
#define NGUOI_H

#include <string>

// ============================================================
// Lop co so TRUU TUONG (Abstract Base Class) cho toan bo he thong nguoi dung.
// Khong the khoi tao truc tiep vi co ham thuan ao hienThiMenu().
// Cac lop dan xuat: ChuCuaHang, NhanVienBanHang, KhachHang
// ============================================================
class Nguoi {
protected:
    std::string ID;
    std::string matKhau;

public:
    Nguoi();
    Nguoi(const std::string& id, const std::string& mk);

    // Destructor ao - BAT BUOC de giai phong dung lop dan xuat khi
    // xoa doi tuong thong qua con tro lop co so (Nguoi*)
    virtual ~Nguoi();

    // ----- Getter / Setter -----
    std::string getID() const;
    std::string getMatKhau() const;
    void setID(const std::string& id);
    void setMatKhau(const std::string& mk);

    // ----- Ham thuan ao (Pure Virtual Function) -----
    // Moi lop con BAT BUOC phai hien thuc lai theo dac thu vai tro cua minh
    virtual void hienThiMenu() = 0;
};

#endif // NGUOI_H