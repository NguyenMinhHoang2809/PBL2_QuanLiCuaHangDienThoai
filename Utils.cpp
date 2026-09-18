#include "Utils.h"
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
// timDuongDanTheoSoSeri(): ham NOI BO (khong khai bao trong Utils.h),
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

// ------------------------------------------------------------
// pauseScreen(): cho nguoi dung nhan Enter truoc khi menu goi
// clearScreen() xoa sach ket qua vua in. Diem mau chot: TRUOC do,
// mot so cho (vd: sau nhapSoNguyen()/nhapSoThucKhongAm()) da tu
// cin.ignore() san nen buffer dang SACH, con mot so cho khac (vd:
// sau cin >> chuoi trong Account.cpp) van con ky tu '\n' SOT LAI.
// Neu goi thang cin.ignore() ma buffer dang sach, ham se BI TREO
// (doi nguoi dung go them 1 lan nua ma khong hien thi gi) - vi vay
// PHAI kiem tra cin.rdbuf()->in_avail() (so ky tu dang san sang
// doc ma KHONG can cho I/O moi) truoc, chi ignore() khi thuc su
// con rac trong buffer.
// ------------------------------------------------------------
void pauseScreen() {
    cout << "\nNhan Enter de tiep tuc...";

    if (cin.rdbuf()->in_avail() > 0) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.get();
}

// ------------------------------------------------------------
// nhapSoNguyen(): boc "cin >> giaTri" (kieu int) trong vong lap.
// Neu nguoi dung go chu, cin chuyen sang failbit va KHONG gan gia
// tri cho giaTri -> bat buoc phai cin.clear() (xoa co loi) roi
// cin.ignore(...) (don sach ky tu rac con ket trong buffer), neu
// khong lan cin >> ke tiep se doc lai dung ky tu rac do va lap loi
// vo han (treo cung Console).
// ------------------------------------------------------------
int nhapSoNguyen(const string& thongBao) {
    int giaTri;
    while (true) {
        cout << thongBao;
        cin >> giaTri;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Du lieu khong hop le! Vui long nhap lai so nguyen.\n";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return giaTri;
    }
}

// ------------------------------------------------------------
// nhapSoThucKhongAm(): tuong tu nhapSoNguyen() nhung cho kieu double
// va bo sung kiem tra khong cho phep gia tri am (dung cho Gia Nhap /
// Gia Ban - khong the co gia san pham am).
// ------------------------------------------------------------
double nhapSoThucKhongAm(const string& thongBao) {
    double giaTri;
    while (true) {
        cout << thongBao;
        cin >> giaTri;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Du lieu khong hop le! Vui long nhap lai so.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (giaTri < 0) {
            cout << "Gia tri khong duoc am! Vui long nhap lai.\n";
            continue;
        }

        return giaTri;
    }
}