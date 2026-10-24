#ifndef TienIch_H
#define TienIch_H

#include <string>
#include <vector>

// ============================================================
// TienIch - tap hop cac ham tien ich DUNG CHUNG cho toan he thong,
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
// TienIch vi chuc nang "nhap hang loat tu file" cung can dung (nguyen
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

// Ve nut "Quay lai" (cung kieu dang voi nut Menu) ben duoi ket qua cua
// mot chuc nang va cho nguoi dung click chuot trai vao nut / nhan ESC
// (hoac Enter) roi moi quay lai menu - dam bao nguoi dung KIP DOC ket
// qua truoc khi man hinh bi xoa sach.
void pauseScreen();

// ------------------------------------------------------------
// HUY THAO TAC: khi dang o bat ky o nhap lieu nao (nhapChuoiForm,
// nhapSoNguyen, nhapSoThucKhongAm...), neu nguoi dung click chuot trai
// vao nut "Quay lai" hoac nhan phim ESC, ham nhap se NEM ngoai le
// HuyThaoTac (bo qua moi kiem tra rong / dinh dang). Menu cua tung vai
// tro (hienThiMenu) bat ngoai le nay va hien lai menu ngay lap tuc.
// ------------------------------------------------------------
struct HuyThaoTac {};

// ------------------------------------------------------------
// Cac ham boc loi nhap lieu (input validation) - hien thi o nhap
// phang can giua man hinh, chong treo cung khi nguoi dung go chu
// vao bien so, va chong gia tri am cho gia SP.
// ------------------------------------------------------------

// Nhap mot so nguyen hop le tu ban phim (trong khung Form can giua).
int nhapSoNguyen(const std::string& thongBao);

// Nhap mot so thuc >= 0 tu ban phim (trong khung Form can giua).
double nhapSoThucKhongAm(const std::string& thongBao);

// ------------------------------------------------------------
// Ho tro tieng Viet + Bo ham ve FORM NHAP LIEU PHANG (FLAT UI)
// can giua man hinh Console
// ------------------------------------------------------------

// Goi 1 LAN DUY NHAT o dau main(): chuyen Console sang UTF-8 (in duoc
// tieng Viet co dau), bat che do ma ANSI va tu dong cai dat hinh nen
// Windows Terminal neu co file background.jpg / background.png.
void khoiTaoConsole();

// Tu dong kiem tra file background.jpg (hoac background.png) trong thu
// muc du an va cap nhat settings.json cua Windows Terminal.
void caiDatHinhNenWindowsTerminal();

// Dem so ky tu HIEN THI thuc te (so codepoint UTF-8) cua chuoi.
int doRongHienThi(const std::string& s);

// Lay chieu rong hien tai cua cua so Console (so cot ky tu) va tinh
// so khoang trang le trai de can giua mot khoi co do rong `chieuRongKhoi`.
int layChieuRongConsole();
int tinhLeTraiCanGiua(int chieuRongKhoi);

// Cac loai thong bao hien thi ben duoi Form
enum LoaiThongBaoForm {
    LOAI_THONG_TIN,   // Chu trang sang
    LOAI_GHI_CHU,     // Chu xam nhat (huong dan / ghi chu mo)
    LOAI_LOI,         // Chu do sang in dam kem (!)
    LOAI_THANH_CONG   // Chu xanh la sang in dam kem [OK]
};

// Ve Header / Tieu de cua Form dang khoi phang (Flat UI) 3 dong can
// giua man hinh Console (nen xam dam, chu do sang in dam dong bo voi
// Menu chinh), kem dong ghi chu mo tuy chon ngay phia duoi.
void veTieuDeForm(const std::string& tieuDe,
                  const std::string& ghiChu = "",
                  int rongKhung = 56);

// Ve 1 dong van ban bat ky can giua man hinh Console (tinh dung do
// dai UTF-8), co the truyen ma mau ANSI tuy chon.
void inDongCanGiua(const std::string& noiDung, const char* maMauANSI = "");

// Ve 1 dong nam gon trong hop phang can giua co do rong `rongKhung`
// (dung de hien thi danh sach file, thong tin chi tiet hoa don...).
void inDongTrongKhungForm(const std::string& noiDung,
                          int rongKhung = 56,
                          const char* maMauANSI = "\033[48;5;236m\033[97m");

// In thong bao Loi (do) / Thanh cong (xanh la) / Ghi chu (xam) can
// giua ngay ben duoi khung Form.
void inThongBaoForm(const std::string& noiDung,
                    LoaiThongBaoForm loai = LOAI_THONG_TIN);

// Hien thi lai 1 truong da nhap trong khung Form phang can giua (vd:
// hien lai o ID sau khi da nhap dung ID va dang nhap lai Mat khau).
void inTruongDaNhapForm(const std::string& nhan,
                        const std::string& giaTri,
                        int rongKhung = 56,
                        int rongNhan = 22);

// Ve 1 dong nhap lieu (Input field) phang can giua man hinh:
//   - Nhan (label) nam trong cot trai co do rong co dinh `rongNhan`
//     (thang hang doc giua tat ca cac truong).
//   - O nhap nam ben phai, dat con tro ngay ben trong khung de nguoi
//     dung go truc tiep.
//   - Neu `thongBaoLoi` khong rong: in thong bao loi mau do can giua
//     ngay ben duoi khung truoc khi dua con tro quay ve o nhap.
std::string nhapChuoiForm(const std::string& nhan,
                          const std::string& thongBaoLoi = "",
                          bool choPhepRong = false,
                          int rongKhung = 56,
                          int rongNhan = 22);

// Xoa `soDong` dong phia tren vi tri con tro hien tai (dung khi 1 o
// nhap trong Form bi sai dieu kien va can ve de lai ngay tai cho).
void xoaDongTruocForm(int soDong = 1);

// Ham tuong thich nguoc (goi nhapChuoiForm can giua).
std::string nhapDongKemThongBaoLoi(const std::string& nhanDe, const std::string& thongBaoLoi);

// ------------------------------------------------------------
// MENU NUT BAM KHOI PHANG (FLAT UI - chon bang chuot + ban phim)
// ------------------------------------------------------------
int chonMenu(const std::string& tieuDe, const std::vector<std::string>& cacMuc);
int chonMenuConsole(const std::string& tieuDe, const std::vector<std::string>& cacMuc);

#endif // TienIch_H
