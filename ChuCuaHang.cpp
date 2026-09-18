#include "ChuCuaHang.h"
#include "NhapHangHoa.h"   // TaoMatHang(), NhapHangLoatTuFile()
#include "Account.h"       // TaoAccount()
#include "Utils.h"         // nhapSoNguyen()
#include "KhoHang.h"       // XuatDanhSachKhoHang()
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>

using namespace std;
namespace fs = std::filesystem;

ChuCuaHang::ChuCuaHang() : Nguoi() {}

// Constructor 2 tham so - goi lai constructor cua lop cha Nguoi(id, mk)
ChuCuaHang::ChuCuaHang(string id, string mk) : Nguoi(id, mk) {}

ChuCuaHang::~ChuCuaHang() {}

// ------------------------------------------------------------
// phanTichNgay(): tach chuoi "dd-mm-yyyy" (dinh dang duy nhat ma
// Utils::layNgayHeThong() va NhanVienBanHang.cpp dang dung) thanh 3
// so nguyen ngay/thang/nam. Tra ve false neu chuoi khong dung dinh
// dang (thieu dau '-', phan nao khong phai so...) - dung de bo qua
// an toan cac hoa don co Ngay Lap bi loi/thieu, khong lam crash
// chuong trinh khi loc theo thoi gian.
// ------------------------------------------------------------
static bool phanTichNgay(const string& ngayLap, int& ngay, int& thang, int& nam) {
    size_t viTriDau = ngayLap.find('-');
    if (viTriDau == string::npos) return false;

    size_t viTriHai = ngayLap.find('-', viTriDau + 1);
    if (viTriHai == string::npos) return false;

    try {
        ngay = stoi(ngayLap.substr(0, viTriDau));
        thang = stoi(ngayLap.substr(viTriDau + 1, viTriHai - viTriDau - 1));
        nam = stoi(ngayLap.substr(viTriHai + 1));
    } catch (...) {
        return false;
    }
    return true;
}

// ------------------------------------------------------------
// Chuc nang: Xem doanh thu / danh sach hoa don cua 1 Nhan Vien cu the,
// co the loc theo Ngay cu the / Thang / Nam / Xem tat ca.
// "Doanh Thu" o day tinh theo cong thuc nguoi dung yeu cau: tong
// (Gia Ban - Gia Nhap) cua tung san pham trong hoa don - ve ban chat
// la LOI NHUAN GOP, da duoc HoaDon::luuFile() tinh san va luu vao
// truong "Doanh Thu" ngay luc lap hoa don (xem NhanVienBanHang.cpp).
// ------------------------------------------------------------
static void XemDoanhThuNhanVien() {
    string maNhanVien;
    // Lap lai cho den khi nhap duoc Ma Nhan Vien CO THAT SU TON TAI -
    // tranh go nham ID dan den bao cao "khong co hoa don nao", gay
    // hieu lam la nhan vien khong ban duoc gi.
    while (true) {
        cout << "Nhap Ma Nhan Vien: ";
        cin >> maNhanVien;

        if (!fs::exists("DataNhanVien/" + maNhanVien + ".txt")) {
            cout << "Khong ton tai tai khoan Nhan Vien nay! Vui long nhap lai.\n";
            continue;
        }
        break;
    }

    cout << "\nLoc bao cao theo:\n";
    cout << "1. Ngay cu the (dd-mm-yyyy)\n";
    cout << "2. Thang (mm-yyyy)\n";
    cout << "3. Nam (yyyy)\n";
    cout << "4. Tat ca (khong loc thoi gian)\n";
    int locTheo = nhapSoNguyen("Lua chon: ");

    int ngayLoc = 0, thangLoc = 0, namLoc = 0;
    if (locTheo == 1) {
        ngayLoc = nhapSoNguyen("Nhap ngay (dd): ");
        thangLoc = nhapSoNguyen("Nhap thang (mm): ");
        namLoc = nhapSoNguyen("Nhap nam (yyyy): ");
    } else if (locTheo == 2) {
        thangLoc = nhapSoNguyen("Nhap thang (mm): ");
        namLoc = nhapSoNguyen("Nhap nam (yyyy): ");
    } else if (locTheo == 3) {
        namLoc = nhapSoNguyen("Nhap nam (yyyy): ");
    }
    // locTheo == 4 (hoac gia tri khac): khong loc, xem toan bo hoa don

    string thuMucHoaDon = "DataHoaDon";
    double tongDoanhThu = 0;
    double tongDoanhSo = 0; // = tong Tong Tien (doanh thu ke toan chuan, de doi chieu)
    int soLuongHoaDon = 0;

    cout << "\n===== HOA DON CUA NHAN VIEN " << maNhanVien << " =====\n";

    if (fs::exists(thuMucHoaDon) && fs::is_directory(thuMucHoaDon)) {
        for (const auto& entry : fs::directory_iterator(thuMucHoaDon)) {
            if (!entry.is_regular_file()) continue;

            ifstream fileDoc(entry.path());
            if (!fileDoc.is_open()) continue;

            string dong, maHD = "", maKH = "", ngayLap = "", tongTienStr = "", doanhThuStr = "";
            bool thuocVeNhanVienNay = false;

            while (getline(fileDoc, dong)) {
                if (dong.rfind("Ma Hoa Don:", 0) == 0) {
                    maHD = dong.substr(dong.find(":") + 1);
                } else if (dong.rfind("Ma Khach Hang:", 0) == 0) {
                    maKH = dong.substr(dong.find(":") + 1);
                } else if (dong.rfind("Ma Nhan Vien:", 0) == 0) {
                    string maNV = dong.substr(dong.find(":") + 1);
                    size_t vt = maNV.find_first_not_of(" \t");
                    if (vt != string::npos) maNV = maNV.substr(vt);
                    if (maNV == maNhanVien) thuocVeNhanVienNay = true;
                } else if (dong.rfind("Ngay Lap:", 0) == 0) {
                    ngayLap = dong.substr(dong.find(":") + 1);
                    size_t vt = ngayLap.find_first_not_of(" \t");
                    if (vt != string::npos) ngayLap = ngayLap.substr(vt);
                } else if (dong.rfind("Tong Tien:", 0) == 0) {
                    tongTienStr = dong.substr(dong.find(":") + 1);
                } else if (dong.rfind("Doanh Thu:", 0) == 0) {
                    doanhThuStr = dong.substr(dong.find(":") + 1);
                }
            }
            fileDoc.close();

            if (!thuocVeNhanVienNay) continue;

            // Loc theo thoi gian neu nguoi dung chon 1/2/3 (bo qua neu
            // Ngay Lap trong file bi loi dinh dang - an toan, khong crash)
            if (locTheo == 1 || locTheo == 2 || locTheo == 3) {
                int ngay, thang, nam;
                if (!phanTichNgay(ngayLap, ngay, thang, nam)) continue;

                if (locTheo == 1 && (ngay != ngayLoc || thang != thangLoc || nam != namLoc)) continue;
                if (locTheo == 2 && (thang != thangLoc || nam != namLoc)) continue;
                if (locTheo == 3 && (nam != namLoc)) continue;
            }

            cout << "- " << maHD << " | KH:" << maKH << " | Ngay:" << ngayLap
                 << " | Tong Tien:" << tongTienStr << " | Doanh Thu:" << doanhThuStr << "\n";

            soLuongHoaDon++;
            try {
                if (!tongTienStr.empty()) tongDoanhSo += stod(tongTienStr);
                if (!doanhThuStr.empty()) tongDoanhThu += stod(doanhThuStr);
            } catch (...) {
                // Hoa don cu/loi dinh dang: bo qua, khong lam sai tong cua
                // cac hoa don con lai.
            }
        }
    }

    if (soLuongHoaDon == 0) {
        cout << "Khong tim thay hoa don nao phu hop.\n";
        return;
    }

    cout << "----------------------------------------\n";
    cout << "So luong hoa don : " << soLuongHoaDon << "\n";
    cout << fixed << setprecision(0);
    cout << "Tong doanh so    : " << tongDoanhSo << " (tong Gia Ban - doanh thu ke toan chuan)\n";
    cout << "Tong doanh thu   : " << tongDoanhThu << " (tong Gia Ban - Gia Nhap, theo yeu cau)\n";
}

// ------------------------------------------------------------
// Chuc nang: "Xoa" tai khoan Nhan Vien - KHONG xoa file/database, chi
// GHI DE mat khau hien tai thanh mat khau mac dinh "31415". Ly do giu
// nguyen file: cac hoa don da lap boi nhan vien nay (Ma Nhan Vien: ...)
// van can tra cuu duoc binh thuong trong bao cao doanh thu ben tren -
// neu xoa han file, ID do se "mo coi" khoi DataNhanVien va cac kiem
// tra fs::exists("DataNhanVien/...") o noi khac se sai lech.
// ------------------------------------------------------------
static void XoaTaiKhoanNhanVien() {
    string maNhanVien;
    while (true) {
        cout << "Nhap Ma Nhan Vien can xoa (vo hieu hoa): ";
        cin >> maNhanVien;

        if (!fs::exists("DataNhanVien/" + maNhanVien + ".txt")) {
            cout << "Khong ton tai tai khoan Nhan Vien nay! Vui long nhap lai.\n";
            continue;
        }
        break;
    }

    string tenFile = "DataNhanVien/" + maNhanVien + ".txt";

    ofstream file(tenFile); // Mo o che do ghi -> tu dong xoa toan bo noi dung cu
    if (!file.is_open()) {
        cout << "Khong the cap nhat tai khoan nay!\n";
        return;
    }

    // Giu nguyen dinh dang 2 dong (ID / Mat Khau) dung nhu Account.cpp
    // da ghi luc TaoAccount(), chi thay doi gia tri mat khau.
    file << maNhanVien << "\n31415\n";
    file.close();

    cout << "Da vo hieu hoa tai khoan " << maNhanVien
         << " (mat khau da bi dat lai ve mac dinh, nhan vien khong con dang nhap duoc bang mat khau cu).\n";
}

void ChuCuaHang::hienThiMenu() {
    int luaChon;
    do {
        clearScreen(); // Dọn màn hình trước khi vẽ lại menu
        cout << "\n===== MENU CHU CUA HANG (" << getID() << ") =====\n";
        cout << "1. Nhap hang hoa moi (1 thiet bi)\n";
        cout << "2. Nhap hang LOAT tu file\n";
        cout << "3. Xuat danh sach kho hang\n";
        cout << "4. Tao tai khoan Nhan vien\n";
        cout << "5. Xem doanh thu / hoa don theo Nhan Vien\n";
        cout << "6. Xoa (vo hieu hoa) tai khoan Nhan Vien\n";
        cout << "0. Dang xuat\n";
        luaChon = nhapSoNguyen("Lua chon: ");

        switch (luaChon) {
            case 1:
                clearScreen();
                cout << "===== NHAP HANG HOA MOI =====\n\n";
                TaoMatHang();
                pauseScreen();
                break;
            case 2:
                clearScreen();
                cout << "===== NHAP HANG LOAT TU FILE =====\n";
                NhapHangLoatTuFile();
                pauseScreen();
                break;
            case 3:
                clearScreen();
                // true = Quan Ly duoc xem CA Gia Nhap
                XuatDanhSachKhoHang(true);
                pauseScreen();
                break;
            case 4:
                clearScreen();
                cout << "===== TAO TAI KHOAN NHAN VIEN =====\n\n";
                TaoAccount("QL");
                pauseScreen();
                break;
            case 5:
                clearScreen();
                XemDoanhThuNhanVien();
                pauseScreen();
                break;
            case 6:
                clearScreen();
                cout << "===== XOA (VO HIEU HOA) TAI KHOAN NHAN VIEN =====\n\n";
                XoaTaiKhoanNhanVien();
                pauseScreen();
                break;
            case 0:
                cout << "Dang xuat...\n";
                break;
            default:
                cout << "Lua chon khong hop le!\n";
                pauseScreen();
        }
    } while (luaChon != 0);
}