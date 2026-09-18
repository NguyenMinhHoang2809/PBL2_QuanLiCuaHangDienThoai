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
// Neu tim thay: gan tenMatHangRa, giaNhapRa, giaBanRa, daBanRoi (true
// neu san pham nay da co "Tinh Trang: Da Ban" trong file) va tra ve
// true. giaNhapRa can thiet de tinh "Doanh Thu" (Gia Ban - Gia Nhap)
// cua tung hoa don khi lap don.
// Neu khong tim thay hoac du lieu file bi loi: tra ve false.
bool timMatHangTheoSoSeri(const std::string& soSeri, std::string& tenMatHangRa,
                            double& giaNhapRa, double& giaBanRa, bool& daBanRoi);

// Cap nhat file mat hang (DataMatHang/<TenMatHang>/<SoSeri>.txt) sang
// trang thai "Da Ban", ghi kem ma khach hang da mua. Duoc goi NGAY SAU
// khi mot hoa don chua so seri nay duoc luuFile() thanh cong.
void danhDauMatHangDaBan(const std::string& soSeri, const std::string& maKhachHang);

// Kiem tra So Seri da ton tai o BAT KY dau trong toan bo DataMatHang hay
// chua - So Seri la duy nhat tren TOAN HE THONG, khong phu thuoc dong
// may/thu muc con nao (2 dien thoai khac dong khong the trung seri).
// Duoc goi boi NhapHangHoa.cpp truoc khi tao mat hang moi.
bool soSeriDaTonTai(const std::string& soSeri);

// Bo TOAN BO khoang trang trong ten mat hang de dung lam TEN THU MUC.
// Truoc day ham nay nam static trong NhapHangHoa.cpp, nay chuyen ra
// Utils vi chuc nang "nhap hang loat tu file" cung can dung (nguyen
// tac DRY) - 2 noi phai chuan hoa GIONG HET NHAU, neu khong cung mot
// san pham se bi tach thanh 2 thu muc khac nhau.
std::string chuanHoaTenThuMuc(const std::string& ten);

// Dinh dang so tien co dau cham phan cach hang nghin (vd: 15000000
// -> "15.000.000") cho de doc khi xuat bang danh sach kho hang.
std::string dinhDangTien(double soTien);

// ------------------------------------------------------------
// Dieu khien man hinh Console - giup giao dien do doi menu tro nen
// gon gang, chuyen nghiep thay vi lich su cu chong chat nhau.
// ------------------------------------------------------------

// Xoa sach man hinh Console (Windows: cls, Linux/macOS: clear).
void clearScreen();

// Dung lai va cho nguoi dung nhan Enter truoc khi tiep tuc - dung
// SAU khi in ket qua mot chuc nang, TRUOC khi quay lai menu (menu se
// goi clearScreen() ngay sau do) - dam bao nguoi dung KIP DOC ket qua
// truoc khi man hinh bi xoa sach.
void pauseScreen();

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