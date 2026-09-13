#ifndef UTILS_H
#define UTILS_H

#include <string>

// ============================================================
// Utils - tap hop cac ham tien ich DUNG CHUNG cho toan he thong,
// tranh lap code (nguyen tac DRY) giua NhanVienBanHang va KhachHang
// (ca hai deu can lap Hoa Don khi ban/mua hang).
// ============================================================

// Lay ngay hien tai theo dinh dang dd-mm-yyyy tu dong ho he thong.
std::string layNgayHeThong();

// Tim mat hang theo So Seri trong toan bo thu muc con cua DataMatHang
// (duyet de quy vi khong biet truoc ten thu muc con - TenMatHang).
// Neu tim thay: gan tenMatHangRa, giaBanRa va tra ve true.
// Neu khong tim thay hoac du lieu file bi loi: tra ve false.
bool timMatHangTheoSoSeri(const std::string& soSeri, std::string& tenMatHangRa, double& giaBanRa);

// ------------------------------------------------------------
// Cac ham boc loi nhap lieu (input validation) - dung chung cho
// moi vi tri "cin >> bienSo" trong toan he thong, chong treo cung
// khi nguoi dung go chu vao bien so, va chong gia tri am cho gia SP.
// ------------------------------------------------------------

// Nhap mot so nguyen hop le tu ban phim. Lap lai neu du lieu khong
// phai la so (vd: go chu) - khong bao gio tra ve gia tri "rac".
int nhapSoNguyen(const std::string& thongBao);

// Nhap mot so thuc >= 0 tu ban phim. Lap lai neu du lieu khong phai
// la so, hoac neu nguoi dung nhap gia tri am (khong hop le cho Gia
// Nhap / Gia Ban).
double nhapSoThucKhongAm(const std::string& thongBao);

#endif // UTILS_H