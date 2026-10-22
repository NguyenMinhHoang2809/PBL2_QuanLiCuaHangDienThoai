#include "TienIch.h"
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <limits>
#include <vector>
#include <algorithm>
#include <cstdlib>

#ifdef _WIN32
// NOMINMAX: chan windows.h dinh nghia macro min/max, neu khong se lam
// hong numeric_limits<streamsize>::max() ben duoi.
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// MinGW doi cu co the chua dinh nghia hang so nay
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

using namespace std;
namespace fs = std::filesystem;

// ------------------------------------------------------------
// layNgayHeThong(): lay ngay hien tai theo dinh dang dd-mm-yyyy.
// Dung ostringstream + setw/setfill (thuan C++) thay vi snprintf
// de tranh canh bao -Wformat-truncation cua GCC.
// ------------------------------------------------------------
string layNgayHeThong() {
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);

    ostringstream oss;
    oss << setw(2) << setfill('0') << ltm->tm_mday << "-"
        << setw(2) << setfill('0') << (1 + ltm->tm_mon) << "-"
        << setw(4) << setfill('0') << (1900 + ltm->tm_year);
    return oss.str();
}

// ------------------------------------------------------------
// timDuongDanTheoSoSeri(): ham NOI BO (khong khai bao trong TienIch.h),
// dung chung cho ca timMatHangTheoSoSeri() (doc) lan danhDauMatHangDaBan()
// (ghi) - tranh lap code duyet recursive_directory_iterator 2 lan.
// Tra ve duong dan file neu tim thay, hoac fs::path() rong neu khong.
// ------------------------------------------------------------
static fs::path timDuongDanTheoSoSeri(const string& soSeri) {
    string thuMucGoc = "DataMatHang";

    if (!fs::exists(thuMucGoc) || !fs::is_directory(thuMucGoc)) {
        return fs::path();
    }

    for (const auto& entry : fs::recursive_directory_iterator(thuMucGoc)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().stem().string() == soSeri) return entry.path();
    }

    return fs::path();
}

// ------------------------------------------------------------
// timMatHangTheoSoSeri(): tim file <soSeri>.txt trong toan bo thu muc
// con cua DataMatHang. Neu tim thay, doc "Ten Mat Hang", "Gia Ban" va
// "Tinh Trang" tra ve qua tham chieu (out parameter) va tra ve true.
// daBanRoi = true neu file co dong "Tinh Trang: Da Ban" - CALLER (noi
// goi ham nay) chiu trach nhiem quyet dinh co cho ban tiep hay khong,
// ham nay chi bao cao trang thai, khong tu y chan.
// ------------------------------------------------------------
bool timMatHangTheoSoSeri(const string& soSeri, string& tenMatHangRa,
                            double& giaNhapRa, double& giaBanRa, bool& daBanRoi) {
    fs::path duongDan = timDuongDanTheoSoSeri(soSeri);
    if (duongDan.empty()) return false;

    ifstream fileDoc(duongDan);
    if (!fileDoc.is_open()) return false;

    string dong, tenMatHang = "", giaNhapStr = "", giaBanStr = "", tinhTrang = "";
    while (getline(fileDoc, dong)) {
        if (dong.rfind("Ten Mat Hang:", 0) == 0) {
            tenMatHang = dong.substr(dong.find(":") + 1);
            size_t viTriDau = tenMatHang.find_first_not_of(" \t");
            if (viTriDau != string::npos) tenMatHang = tenMatHang.substr(viTriDau);
        } else if (dong.rfind("Gia Nhap:", 0) == 0) {
            giaNhapStr = dong.substr(dong.find(":") + 1);
            size_t viTriDau = giaNhapStr.find_first_not_of(" \t");
            if (viTriDau != string::npos) giaNhapStr = giaNhapStr.substr(viTriDau);
        } else if (dong.rfind("Gia Ban:", 0) == 0) {
            giaBanStr = dong.substr(dong.find(":") + 1);
            size_t viTriDau = giaBanStr.find_first_not_of(" \t");
            if (viTriDau != string::npos) giaBanStr = giaBanStr.substr(viTriDau);
        } else if (dong.rfind("Tinh Trang:", 0) == 0) {
            tinhTrang = dong.substr(dong.find(":") + 1);
            size_t viTriDau = tinhTrang.find_first_not_of(" \t");
            if (viTriDau != string::npos) tinhTrang = tinhTrang.substr(viTriDau);
        }
    }
    fileDoc.close();

    if (giaBanStr.empty()) return false;

    try {
        giaBanRa = stod(giaBanStr);
        // Gia Nhap co the rong o file rat cu (truoc khi co truong nay) -
        // mac dinh ve 0 thay vi lam that bai toan bo ham, de van tra ve
        // duoc Gia Ban cho cac chuc nang khac dung ham nay.
        giaNhapRa = giaNhapStr.empty() ? 0.0 : stod(giaNhapStr);
    } catch (...) {
        return false; // Gia Nhap/Gia Ban trong file bi loi dinh dang
    }

    tenMatHangRa = tenMatHang;
    // Neu file cu (tao truoc khi co tinh nang nay) khong co dong "Tinh
    // Trang", tinhTrang se rong "" -> mac dinh coi la CHUA BAN, an toan.
    daBanRoi = (tinhTrang == "Da Ban");
    return true;
}

// ------------------------------------------------------------
// danhDauMatHangDaBan(): cap nhat file DataMatHang/.../<SoSeri>.txt
// sang trang thai "Da Ban" + ghi ma khach hang da mua. Doc toan bo
// file vao vector<string>, THAY THE cac dong "Tinh Trang:" / "Ma Khach
// Hang Da Mua:" da co san (neu la file tao boi ban NhapHangHoa.cpp
// moi), hoac THEM MOI 2 dong nay (neu la file cu tao truoc khi co tinh
// nang nay), roi ghi de lai toan bo file.
// ------------------------------------------------------------
void danhDauMatHangDaBan(const string& soSeri, const string& maKhachHang) {
    fs::path duongDan = timDuongDanTheoSoSeri(soSeri);
    if (duongDan.empty()) return; // Khong tim thay thi khong co gi de cap nhat

    vector<string> cacDong;
    string dong;

    ifstream fileDoc(duongDan);
    if (fileDoc.is_open()) {
        while (getline(fileDoc, dong)) {
            cacDong.push_back(dong);
        }
        fileDoc.close();
    }

    bool daCoDongTinhTrang = false;
    bool daCoDongMaKhachHang = false;

    ofstream fileGhi(duongDan); // Mo lai o che do ghi -> tu dong xoa noi dung cu
    if (!fileGhi.is_open()) return;

    for (const string& d : cacDong) {
        if (d.rfind("Tinh Trang:", 0) == 0) {
            fileGhi << "Tinh Trang: Da Ban\n";
            daCoDongTinhTrang = true;
        } else if (d.rfind("Ma Khach Hang Da Mua:", 0) == 0) {
            fileGhi << "Ma Khach Hang Da Mua: " << maKhachHang << "\n";
            daCoDongMaKhachHang = true;
        } else {
            fileGhi << d << "\n";
        }
    }

    // File cu (tao truoc khi co tinh nang nay) se chua co 2 dong tren
    // -> them moi vao cuoi file.
    if (!daCoDongTinhTrang) fileGhi << "Tinh Trang: Da Ban\n";
    if (!daCoDongMaKhachHang) fileGhi << "Ma Khach Hang Da Mua: " << maKhachHang << "\n";

    fileGhi.close();
}

// ------------------------------------------------------------
// soSeriDaTonTai(): tai su dung timDuongDanTheoSoSeri() (ham noi bo o
// tren) - vi ham do da duyet TOAN BO DataMatHang mot cach de quy (bao
// gom moi thu muc con/dong may), nen chi can kiem tra ket qua co rong
// hay khong la biet So Seri da bi chiem chua, KHONG PHU THUOC dong may
// nao dang chua no.
// ------------------------------------------------------------
bool soSeriDaTonTai(const string& soSeri) {
    return !timDuongDanTheoSoSeri(soSeri).empty();
}

// ------------------------------------------------------------
// chuanHoaTenThuMuc(): bo toan bo khoang trang de "iPhone 15 Pro" va
// "iPhone15Pro" cung quy ve DUNG MOT thu muc DataMatHang/iPhone15Pro.
// ------------------------------------------------------------
string chuanHoaTenThuMuc(const string& ten) {
    string ketQua;
    for (char c : ten) {
        if (c != ' ') ketQua += c;
    }
    return ketQua;
}

// ------------------------------------------------------------
// dinhDangTien(): 15000000 -> "15.000.000". Duyet chuoi so tu PHAI
// sang TRAI, cu 3 chu so chen mot dau '.', sau do dao nguoc lai.
// ------------------------------------------------------------
string dinhDangTien(double soTien) {
    long long giaTri = static_cast<long long>(soTien);
    bool laSoAm = (giaTri < 0);
    if (laSoAm) giaTri = -giaTri;

    string chuoiSo = to_string(giaTri);
    string ketQua;
    int dem = 0;

    for (int i = static_cast<int>(chuoiSo.length()) - 1; i >= 0; --i) {
        ketQua += chuoiSo[i];
        dem++;
        if (dem % 3 == 0 && i != 0) ketQua += '.';
    }

    if (laSoAm) ketQua += '-';
    reverse(ketQua.begin(), ketQua.end());
    return ketQua;
}

// ------------------------------------------------------------
// clearScreen(): xoa sach noi dung Console. Dung tien xu ly _WIN32
// de goi dung lenh he dieu hanh - "cls" tren Windows, "clear" tren
// Linux/macOS.
// ------------------------------------------------------------
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ============================================================
// HO TRO TIENG VIET UTF-8 & BO HAM VE FORM NHAP LIEU PHANG (FLAT UI)
// ============================================================

static bool g_hoTroANSI = false;

static const char* const ANSI_LUU_CON_TRO   = "\x1b" "7";   // DECSC
static const char* const ANSI_KHOI_PHUC     = "\x1b" "8";   // DECRC
static const char* const ANSI_MAU_DO        = "\033[91;1m";
static const char* const ANSI_MAU_MAC_DINH  = "\033[0m";

// Bang mau ANSI dong bo giua Menu chinh va Form nhap lieu:
static const char* const ANSI_TIEU_DE      = "\033[48;5;236m\033[91;1m";
static const char* const ANSI_NHAN_FORM    = "\033[48;5;236m\033[97;1m";
static const char* const ANSI_O_NHAP_FORM  = "\033[48;5;238m\033[97m";
static const char* const ANSI_NUT_THUONG   = "\033[48;5;242m\033[97m";
static const char* const ANSI_NUT_SANG     = "\033[48;5;248m\033[97;1m";
static const char* const ANSI_RESET_MAU    = "\033[0m";

void khoiTaoConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD cheDo = 0;
    if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &cheDo)) {
        g_hoTroANSI = SetConsoleMode(hOut, cheDo | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
    }
#else
    g_hoTroANSI = true;
#endif
}

// Dem so ky tu HIEN THI thuc te (so codepoint UTF-8) thay vi .length()
// hay .size() (so byte). Byte dang 10xxxxxx (0x80..0xBF) la byte noi
// tiep cua ky tu UTF-8 nhieu byte -> bo qua khong dem.
int doRongHienThi(const string& s) {
    int dem = 0;
    for (unsigned char c : s) {
        if ((c & 0xC0) != 0x80) dem++;
    }
    return dem;
}

// Cat chuoi UTF-8 de khong vuot qua toiDa ky tu hien thi (khong cat
// ngang giua 1 ky tu nhieu byte).
static string catTheoDoRong(const string& s, int toiDa) {
    if (toiDa <= 0) return "";
    int dem = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if ((c & 0xC0) != 0x80) {
            if (dem == toiDa) return s.substr(0, i);
            dem++;
        }
    }
    return s;
}

int layChieuRongConsole() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (hOut != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hOut, &csbi)) {
        int rong = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        if (rong >= 40) return rong;
    }
#endif
    return 100;
}

int tinhLeTraiCanGiua(int chieuRongKhoi) {
    int rongMH = layChieuRongConsole();
    return max(0, (rongMH - chieuRongKhoi) / 2);
}

void inDongCanGiua(const string& noiDung, const char* maMauANSI) {
    int rong = doRongHienThi(noiDung);
    int leTrai = tinhLeTraiCanGiua(rong);
    cout << string(leTrai, ' ');
    if (maMauANSI && maMauANSI[0] != '\0') {
        cout << maMauANSI << noiDung << ANSI_RESET_MAU << "\n";
    } else {
        cout << noiDung << "\n";
    }
}

void veTieuDeForm(const string& tieuDe, const string& ghiChu, int rongKhung) {
    int rong = max(rongKhung, doRongHienThi(tieuDe) + 8);
    int leTrai = tinhLeTraiCanGiua(rong);
    string khoangLe(leTrai, ' ');

    int rongChu = doRongHienThi(tieuDe);
    int demTrai = max(0, (rong - rongChu) / 2);
    int demPhai = max(0, rong - rongChu - demTrai);

    cout << "\n";
    cout << khoangLe << ANSI_TIEU_DE << string(rong, ' ') << ANSI_RESET_MAU << "\n";
    cout << khoangLe << ANSI_TIEU_DE << string(demTrai, ' ') << tieuDe << string(demPhai, ' ') << ANSI_RESET_MAU << "\n";
    cout << khoangLe << ANSI_TIEU_DE << string(rong, ' ') << ANSI_RESET_MAU << "\n";

    if (!ghiChu.empty()) {
        inDongCanGiua(ghiChu, "\033[90m");
    }
    cout << "\n";
}

void inDongTrongKhungForm(const string& noiDung, int rongKhung, const char* maMauANSI) {
    int leTrai = tinhLeTraiCanGiua(rongKhung);
    string noiDungCat = catTheoDoRong(noiDung, max(0, rongKhung - 4));
    int demPhai = max(0, rongKhung - 2 - doRongHienThi(noiDungCat));
    cout << string(leTrai, ' ') << maMauANSI << "  " << noiDungCat
         << string(demPhai, ' ') << ANSI_RESET_MAU << "\n";
}

void inThongBaoForm(const string& noiDung, LoaiThongBaoForm loai) {
    switch (loai) {
        case LOAI_LOI:
            inDongCanGiua("(!) " + noiDung, "\033[91;1m");
            break;
        case LOAI_THANH_CONG:
            inDongCanGiua("[OK] " + noiDung, "\033[92;1m");
            break;
        case LOAI_GHI_CHU:
            inDongCanGiua(noiDung, "\033[90m");
            break;
        case LOAI_THONG_TIN:
        default:
            inDongCanGiua(noiDung, "\033[97m");
            break;
    }
}

// Bo khoang trang va dau ':' o cuoi nhan de cac truong trong Form luon
// co dinh dang thong nhat "  <Nhan> : "
static string chuanHoaNhanForm(const string& nhan) {
    size_t dau = nhan.find_first_not_of(" \t");
    if (dau == string::npos) return "";
    size_t cuoi = nhan.find_last_not_of(" \t:");
    if (cuoi == string::npos || cuoi < dau) return nhan.substr(dau);
    return nhan.substr(dau, cuoi - dau + 1);
}

void inTruongDaNhapForm(const string& nhan, const string& giaTri, int rongKhung, int rongNhan) {
    string nhanSach = chuanHoaNhanForm(nhan);
    string nhanCat = catTheoDoRong(nhanSach, rongNhan);
    int demNhan = max(0, rongNhan - doRongHienThi(nhanCat));
    int rongCotNhan = rongNhan + 4; // "  " + nhan + ": "
    int rongO = max(10, rongKhung - rongCotNhan);

    string giaTriCat = catTheoDoRong(giaTri, max(0, rongO - 2));
    int demGiaTri = max(0, rongO - 1 - doRongHienThi(giaTriCat));
    int leTrai = tinhLeTraiCanGiua(rongCotNhan + rongO);

    cout << string(leTrai, ' ')
         << ANSI_NHAN_FORM << "  " << nhanCat << string(demNhan, ' ') << ": "
         << ANSI_O_NHAP_FORM << " " << giaTriCat << string(demGiaTri, ' ')
         << ANSI_RESET_MAU << "\n";
}

void xoaDongTruocForm(int soDong) {
    if (soDong <= 0) return;
    for (int i = 0; i < soDong; ++i) {
        cout << "\033[A\r\033[2K";
    }
    cout << flush;
}

string nhapChuoiForm(const string& nhan,
                     const string& thongBaoLoi,
                     bool choPhepRong,
                     int rongKhung,
                     int rongNhan) {
    string nhanSach = chuanHoaNhanForm(nhan);
    string nhanCat = catTheoDoRong(nhanSach, rongNhan);
    int demNhan = max(0, rongNhan - doRongHienThi(nhanCat));
    int rongCotNhan = rongNhan + 4; // "  " + nhan + ": "
    int rongO = max(10, rongKhung - rongCotNhan);
    int tongRong = rongCotNhan + rongO;

    string loiHienTai = thongBaoLoi;

    while (true) {
        int leTrai = tinhLeTraiCanGiua(tongRong);

        if (g_hoTroANSI) {
            cout << string(leTrai, ' ')
                 << ANSI_NHAN_FORM << "  " << nhanCat << string(demNhan, ' ') << ": "
                 << ANSI_O_NHAP_FORM << " " << ANSI_LUU_CON_TRO
                 << string(max(0, rongO - 1), ' ') << ANSI_RESET_MAU;

            if (!loiHienTai.empty()) {
                string chuoiLoi = "(!) " + loiHienTai;
                int leLoi = tinhLeTraiCanGiua(doRongHienThi(chuoiLoi));
                cout << "\n\r\033[2K" << string(leLoi, ' ')
                     << ANSI_MAU_DO << chuoiLoi << ANSI_RESET_MAU;
            } else {
                cout << "\n\r\033[2K";
            }

            cout << ANSI_KHOI_PHUC << ANSI_O_NHAP_FORM << flush;
        } else {
            if (!loiHienTai.empty()) {
                inThongBaoForm(loiHienTai, LOAI_LOI);
            }
            cout << string(leTrai, ' ') << "  " << nhanCat << string(demNhan, ' ') << ": " << flush;
        }

        string dong;
        if (!getline(cin, dong)) {
            cin.clear();
            cout << ANSI_RESET_MAU << "\n";
            return "";
        }

        if (g_hoTroANSI) {
            // Sau khi nguoi dung nhan Enter, con tro xuong dong ben duoi (dong
            // chua thong bao loi tam thoi) -> xoa sach dong loi nay de truong
            // tiep theo in de len vi tri ngay sat duoi.
            cout << ANSI_RESET_MAU << "\r\033[2K" << flush;
        }

        size_t dau = dong.find_first_not_of(" \t\r");
        string ketQua = "";
        if (dau != string::npos) {
            size_t cuoi = dong.find_last_not_of(" \t\r");
            ketQua = dong.substr(dau, cuoi - dau + 1);
        }

        if (ketQua.empty() && !choPhepRong) {
            if (g_hoTroANSI) xoaDongTruocForm(1);
            loiHienTai = "Vui lòng không để trống trường này!";
            continue;
        }

        return ketQua;
    }
}

std::string nhapDongKemThongBaoLoi(const std::string& nhanDe, const std::string& thongBaoLoi) {
    return nhapChuoiForm(nhanDe, thongBaoLoi, true);
}

// ------------------------------------------------------------
// pauseScreen(): in dong huong dan can giua man hinh va cho Enter.
// ------------------------------------------------------------
void pauseScreen() {
    cout << "\n";
    string thongBao = "Nhấn Enter để tiếp tục...";
    int leTrai = tinhLeTraiCanGiua(doRongHienThi(thongBao));
    cout << string(leTrai, ' ') << "\033[90m" << thongBao << ANSI_RESET_MAU << flush;

    if (cin.rdbuf()->in_avail() > 0) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.get();
}

// ------------------------------------------------------------
// nhapSoNguyen(): hien thi o nhap phang can giua man hinh va kiem tra
// dinh dang so nguyen. Neu nhap sai, bao loi mau do ngay duoi o nhap.
// ------------------------------------------------------------
int nhapSoNguyen(const string& thongBao) {
    string loi = "";
    while (true) {
        string chuoi = nhapChuoiForm(thongBao, loi, false);
        try {
            size_t viTri = 0;
            int giaTri = stoi(chuoi, &viTri);
            if (viTri == chuoi.size()) {
                return giaTri;
            }
        } catch (...) {}

        if (g_hoTroANSI) xoaDongTruocForm(1);
        loi = "Dữ liệu không hợp lệ! Vui lòng nhập lại số nguyên.";
    }
}

// ------------------------------------------------------------
// nhapSoThucKhongAm(): hien thi o nhap phang can giua man hinh va kiem
// tra so thuc >= 0. Neu nhap chu hoac so am, bao loi mau do ngay ben
// duoi khung nhap.
// ------------------------------------------------------------
double nhapSoThucKhongAm(const string& thongBao) {
    string loi = "";
    while (true) {
        string chuoi = nhapChuoiForm(thongBao, loi, false);
        try {
            size_t viTri = 0;
            double giaTri = stod(chuoi, &viTri);
            if (viTri == chuoi.size()) {
                if (giaTri < 0) {
                    if (g_hoTroANSI) xoaDongTruocForm(1);
                    loi = "Giá trị không được âm! Vui lòng nhập lại.";
                    continue;
                }
                return giaTri;
            }
        } catch (...) {}

        if (g_hoTroANSI) xoaDongTruocForm(1);
        loi = "Dữ liệu không hợp lệ! Vui lòng nhập lại số.";
    }
}

// ============================================================
// MENU NUT BAM TREN CONSOLE (chuot + ban phim) - THIET KE PHANG (FLAT UI)
// ============================================================

// Du phong khi KHONG co Console that (vd: chuyen huong input tu file,
// hoac he dieu hanh khac Windows): menu danh so can giua.
static int chonMenuDangSo(const string& tieuDe, const vector<string>& cacMuc) {
    clearScreen();
    veTieuDeForm(tieuDe);
    for (size_t i = 0; i < cacMuc.size(); ++i) {
        inDongTrongKhungForm(to_string(i + 1) + ". " + cacMuc[i]);
    }
    cout << "\n";
    while (true) {
        int chon = nhapSoNguyen("Lựa chọn");
        if (chon >= 1 && chon <= static_cast<int>(cacMuc.size())) return chon - 1;
        inThongBaoForm("Lựa chọn không hợp lệ!", LOAI_LOI);
    }
}

#ifdef _WIN32

static const int CAO_NUT = 3; // Dong 1: dem tren, Dong 2: chu can giua, Dong 3: dem duoi

struct NutConsole {
    int x, y, rong;
};

// In chuoi UTF-8 (co kem ma mau ANSI) tai toa do (x, y). Dung WriteConsoleW
// (chuyen UTF-8 -> UTF-16) ket hop ENABLE_VIRTUAL_TERMINAL_PROCESSING de
// hien thi dung tieng Viet co dau va ma mau ANSI ma khong bi nhap nhay.
static void inTaiViTri(HANDLE hOut, int x, int y, const string& utf8) {
    COORD viTri = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(hOut, viTri);

    int doDai = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), static_cast<int>(utf8.size()), nullptr, 0);
    if (doDai <= 0) return;
    wstring chuoiRong(doDai, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), static_cast<int>(utf8.size()), &chuoiRong[0], doDai);

    DWORD daGhi = 0;
    WriteConsoleW(hOut, chuoiRong.c_str(), static_cast<DWORD>(chuoiRong.size()), &daGhi, nullptr);
}

// Xoa man hinh bang API Win32 thay vi system("cls"): khong tao tien
// trinh cmd.exe moi (nhanh hon, va cmd.exe co the lam thay doi che do
// Console dang duoc menu thiet lap).
static void xoaManHinhWin32(HANDLE hOut, WORD mau) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) return;

    DWORD soO = static_cast<DWORD>(csbi.dwSize.X) * static_cast<DWORD>(csbi.dwSize.Y);
    COORD goc = { 0, 0 };
    DWORD daGhi = 0;
    FillConsoleOutputCharacterW(hOut, L' ', soO, goc, &daGhi);
    FillConsoleOutputAttribute(hOut, mau, soO, goc, &daGhi);
    SetConsoleCursorPosition(hOut, goc);
}

// Ve 1 khoi phang (Flat UI) 3 dong, KHONG dung ky tu ke khung vien:
//   - Dong 1: khoang trang (' ') dem tren
//   - Dong 2: khoang trang (' ') + Chu can giua (tinh bang doRongHienThi) + khoang trang (' ')
//   - Dong 3: khoang trang (' ') dem duoi
// Ca 3 dong deu co dung `rong` ky tu hien thi -> mep phai thang hang tuyet doi.
// Luon reset mau (\033[0m) ngay cuoi moi dong de khong lem mau ra man hinh den.
static void veKhoiPhang(HANDLE hOut, int x, int y, int rong, const string& nhan, const char* maMau) {
    if (rong <= 0) return;

    string nhanCat = catTheoDoRong(nhan, max(0, rong - 2));
    int rongChu = doRongHienThi(nhanCat);
    int leTrai = max(0, (rong - rongChu) / 2);
    int lePhai = max(0, rong - rongChu - leTrai);

    string dongTrong = string(maMau) + string(rong, ' ') + ANSI_RESET_MAU;
    string dongChu   = string(maMau) + string(leTrai, ' ') + nhanCat + string(lePhai, ' ') + ANSI_RESET_MAU;

    inTaiViTri(hOut, x, y,     dongTrong);
    inTaiViTri(hOut, x, y + 1, dongChu);
    inTaiViTri(hOut, x, y + 2, dongTrong);
}

static void veNutConsole(HANDLE hOut, const NutConsole& nut, const string& nhan, bool sang) {
    veKhoiPhang(hOut, nut.x, nut.y, nut.rong, nhan, sang ? ANSI_NUT_SANG : ANSI_NUT_THUONG);
}

int chonMenuConsole(const string& tieuDe, const vector<string>& cacMuc) {
    const int soMuc = static_cast<int>(cacMuc.size());
    if (soMuc == 0) return -1;

    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // Luu lai TRANG THAI GOC de khoi phuc 100% khi thoat menu
    DWORD cheDoInGoc = 0;
    DWORD cheDoOutGoc = 0;
    CONSOLE_SCREEN_BUFFER_INFO csbiGoc;
    CONSOLE_CURSOR_INFO conTroGoc;
    if (!GetConsoleMode(hIn, &cheDoInGoc) || !GetConsoleMode(hOut, &cheDoOutGoc)
        || !GetConsoleScreenBufferInfo(hOut, &csbiGoc) || !GetConsoleCursorInfo(hOut, &conTroGoc)) {
        return chonMenuDangSo(tieuDe, cacMuc); // Khong phai Console that
    }
    const WORD mauGoc = csbiGoc.wAttributes;

    cout << flush; // Day het noi dung cout dang cho ra truoc khi ve bang API

    // Bat che do xu ly ma mau ANSI tren Console Output
    SetConsoleMode(hOut, cheDoOutGoc | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    // ENABLE_EXTENDED_FLAGS (va KHONG kem ENABLE_QUICK_EDIT_MODE) = tat
    // Quick Edit - neu khong, click chuot se "boi den" van ban thay vi
    // gui su kien chuot cho chuong trinh. Khong co ENABLE_LINE_INPUT /
    // ENABLE_ECHO_INPUT -> phim bam khong bi in ra man hinh.
    SetConsoleMode(hIn, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);

    CONSOLE_CURSOR_INFO anConTro = conTroGoc;
    anConTro.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &anConTro);

    vector<NutConsole> cacNut(soMuc);
    int dangChon = 0;
    int soHang = soMuc; // so hang moi cot (dung cho phim trai/phai khi chia 2 cot)
    int soCot = 1;

    // ---- Ve TOAN BO man hinh menu: chi goi luc vao menu hoac khi doi
    //      kich thuoc cua so (KHONG goi khi di chuot -> khong nhap nhay)
    auto veToanBo = [&]() {
        xoaManHinhWin32(hOut, mauGoc);

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hOut, &csbi);
        const int trai = csbi.srWindow.Left;
        const int tren = csbi.srWindow.Top;
        const int rongMH = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        const int caoMH = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

        // Kich thuoc nut bam thu gon can doi (28 ky tu hien thi; neu nhan
        // dai hon thi tu dong cong them 4 khoang trang dem 2 ben).
        int rongNut = 28;
        for (const string& muc : cacMuc) rongNut = max(rongNut, doRongHienThi(muc) + 4);
        rongNut = min(rongNut, max(10, rongMH - 4));

        // Khung tieu de khoi phang (Flat UI): cat gon 28-32 ky tu hien thi
        // (vua van voi "HOÀNG CỬU BẢO MOBILE" = 20 ky tu + 8 dem = 28,
        // thang hang tuyet doi voi cac nut bam 28 ky tu ben duoi).
        int rongTieuDe = max(doRongHienThi(tieuDe) + 8, 28);
        rongTieuDe = min(rongTieuDe, max(10, rongMH - 4));
        int xTieuDe = trai + max(0, (rongMH - rongTieuDe) / 2);
        veKhoiPhang(hOut, xTieuDe, tren + 1, rongTieuDe, tieuDe, ANSI_TIEU_DE);

        // Chon bo cuc vua man hinh (da xoa dong huong dan thao tac va
        // thu gon khoang trong: cac nut bat dau ngay tu tren + 5).
        const int yBatDau = tren + 5;
        const int caoKhaDung = caoMH - 5 - 1;
        const int cauHinh[4][2] = { {1, 1}, {1, 0}, {2, 1}, {2, 0} }; // {soCot, khoangCach}
        int khoangCach = 0;
        for (int k = 0; k < 4; ++k) {
            soCot = cauHinh[k][0];
            khoangCach = cauHinh[k][1];
            soHang = (soMuc + soCot - 1) / soCot;
            int caoCan = soHang * CAO_NUT + (soHang - 1) * khoangCach;
            if (caoCan <= caoKhaDung) break;
        }
        if (soCot == 2) rongNut = min(rongNut, (rongMH - 6) / 2);

        int caoCan = soHang * CAO_NUT + (soHang - 1) * khoangCach;
        int tongRong = soCot * rongNut + (soCot - 1) * 4;
        int x0 = trai + max(0, (rongMH - tongRong) / 2);
        int y0 = yBatDau + max(0, (caoKhaDung - caoCan) / 2);

        for (int i = 0; i < soMuc; ++i) {
            int cot = i / soHang;  // xep theo cot: het cot trai moi sang cot phai
            int hang = i % soHang;
            cacNut[i].x = x0 + cot * (rongNut + 4);
            cacNut[i].y = y0 + hang * (CAO_NUT + khoangCach);
            cacNut[i].rong = rongNut;
            veNutConsole(hOut, cacNut[i], cacMuc[i], i == dangChon);
        }
    };

    // ---- Doi nut dang chon: CHI ve lai 2 nut bi anh huong
    auto doiChon = [&](int moi) {
        if (moi == dangChon || moi < 0 || moi >= soMuc) return;
        veNutConsole(hOut, cacNut[dangChon], cacMuc[dangChon], false);
        dangChon = moi;
        veNutConsole(hOut, cacNut[dangChon], cacMuc[dangChon], true);
    };

    auto timNutTaiViTri = [&](COORD p) -> int {
        for (int i = 0; i < soMuc; ++i) {
            if (p.X >= cacNut[i].x && p.X < cacNut[i].x + cacNut[i].rong
                && p.Y >= cacNut[i].y && p.Y < cacNut[i].y + CAO_NUT) {
                return i;
            }
        }
        return -1;
    };

    veToanBo();

    int ketQua = -1;
    while (ketQua < 0) {
        INPUT_RECORD suKien;
        DWORD soDoc = 0;
        if (!ReadConsoleInputW(hIn, &suKien, 1, &soDoc) || soDoc == 0) {
            ketQua = soMuc - 1; // Loi doc input: coi nhu chon "thoat"
            break;
        }

        if (suKien.EventType == MOUSE_EVENT) {
            const MOUSE_EVENT_RECORD& chuot = suKien.Event.MouseEvent;
            int viTri = timNutTaiViTri(chuot.dwMousePosition);

            // Re chuot (MOUSE_MOVED) hoac bam (dwEventFlags == 0) trung nut -> lam sang nut
            if ((chuot.dwEventFlags == MOUSE_MOVED || chuot.dwEventFlags == 0) && viTri >= 0) {
                doiChon(viTri);
            }
            // Click chuot trai trung nut -> chon ngay
            if (chuot.dwEventFlags == 0 && (chuot.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) && viTri >= 0) {
                ketQua = viTri;
            }
        } else if (suKien.EventType == KEY_EVENT && suKien.Event.KeyEvent.bKeyDown) {
            switch (suKien.Event.KeyEvent.wVirtualKeyCode) {
                case VK_UP:     doiChon((dangChon - 1 + soMuc) % soMuc); break;
                case VK_DOWN:   doiChon((dangChon + 1) % soMuc); break;
                case VK_LEFT:   if (soCot == 2) doiChon(dangChon - soHang); break;
                case VK_RIGHT:  if (soCot == 2) doiChon(dangChon + soHang); break;
                case VK_RETURN: ketQua = dangChon; break;
                case VK_ESCAPE: ketQua = soMuc - 1; break; // Quy uoc: muc cuoi = thoat
                default: break;
            }
        } else if (suKien.EventType == WINDOW_BUFFER_SIZE_EVENT) {
            veToanBo(); // Cua so doi kich thuoc -> tinh lai bo cuc
        }
    }

    // ---- KHOI PHUC Console ve trang thai nhap lieu binh thuong TRUOC
    //      khi tra ve - code nghiep vu phia sau dung cin/getline.
    xoaManHinhWin32(hOut, mauGoc);
    SetConsoleTextAttribute(hOut, mauGoc);
    SetConsoleMode(hIn, cheDoInGoc);          // bat lai Quick Edit, Line/Echo input
    SetConsoleCursorInfo(hOut, &conTroGoc);   // hien lai con tro nhap
    FlushConsoleInputBuffer(hIn);             // bo su kien chuot/phim con sot (vd: nha phim Enter)

    return ketQua;
}

#else // Khong phai Windows: dung menu danh so

int chonMenuConsole(const string& tieuDe, const vector<string>& cacMuc) {
    return chonMenuDangSo(tieuDe, cacMuc);
}

#endif // _WIN32

// Ban mac dinh: chonMenu() = giao dien Console. Khi bien dich voi
// -DDUNG_SDL, chonMenu() duoc dinh nghia trong MenuSDL.cpp thay the.
#ifndef DUNG_SDL
int chonMenu(const string& tieuDe, const vector<string>& cacMuc) {
    return chonMenuConsole(tieuDe, cacMuc);
}
#endif
