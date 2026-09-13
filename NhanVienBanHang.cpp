#include "NhanVienBanHang.h"
#include "Account.h"        // TaoAccount()
#include "HoaDon.h"         // HoaDon, luuFile()
#include "Utils.h"          // layNgayHeThong(), timMatHangTheoSoSeri()
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <set>

using namespace std;
namespace fs = std::filesystem;

NhanVienBanHang::NhanVienBanHang() : Nguoi() {}

// Constructor 2 tham so - goi lai constructor cua lop cha Nguoi(id, mk)
NhanVienBanHang::NhanVienBanHang(string id, string mk) : Nguoi(id, mk) {}

NhanVienBanHang::~NhanVienBanHang() {}

// ------------------------------------------------------------
// Chuc nang: Lap hoa don ban hang
// ------------------------------------------------------------
static void LapHoaDonBanHang() {
    // Lap lai cho den khi nhap duoc Ma Hoa Don CHUA TON TAI trong
    // DataHoaDon/ - tranh truong hop go trung ma hoa don cu, khien
    // ofstream trong HoaDon::luuFile() GHI DE va XOA MAT hoa don cu
    // ma khong he co canh bao nao.
    string maHoaDon;
    while (true) {
        cout << "Nhap Ma Hoa Don: ";
        cin >> maHoaDon;

        if (fs::exists("DataHoaDon/HD_" + maHoaDon + ".txt")) {
            cout << "Ma Hoa Don nay da ton tai! Vui long nhap ma khac.\n";
            continue;
        }
        break;
    }

    string maKhachHang;
    cout << "Nhap Ma Khach Hang: "; cin >> maKhachHang;

    double tongTien = 0;
    string soSeri;

    // Luu lai tap hop So Seri DA THEM vao hoa don nay - moi may dien thoai
    // that te chi co DUY NHAT 1 so seri, khong the ban 2 lan cung 1 may
    // trong CUNG 1 hoa don. set<string> giup kiem tra "da ton tai chua"
    // voi do phuc tap O(log n), nhanh hon duyet vector tuyen tinh.
    set<string> daThemSoSeri;

    cout << "\n--- Nhap So Seri mat hang (nhap 0 de ket thuc va chot don) ---\n";
    do {
        cout << "So Seri: ";
        cin >> soSeri;

        if (soSeri == "0") break;

        // Kiem tra trung lap TRUOC KHI tim trong DataMatHang - tranh cong
        // don gia ban 2 lan cho cung 1 san pham vao tongTien.
        if (daThemSoSeri.count(soSeri) > 0) {
            cout << "So seri nay da duoc them vao hoa don roi, khong the them lai!\n";
            continue;
        }

        string tenMatHang;
        double giaBan;

        if (timMatHangTheoSoSeri(soSeri, tenMatHang, giaBan)) {
            tongTien += giaBan;
            daThemSoSeri.insert(soSeri); // Danh dau seri nay DA duoc them
            cout << "Da them " << tenMatHang << " - Gia: "
                 << fixed << setprecision(0) << giaBan << "\n";
        } else {
            cout << "Khong ton tai so seri nay!\n";
        }

    } while (soSeri != "0");

    if (tongTien <= 0) {
        cout << "Hoa don chua co mat hang nao, huy lap hoa don!\n";
        return;
    }

    // Cho phep nhan vien tu nhap ngay lap, neu de trong se lay ngay he thong
    cin.ignore(); // bo qua ky tu '\n' con sot lai sau lan cin >> soSeri cuoi cung
    string ngayLap;
    cout << "Nhap Ngay Lap (dd-mm-yyyy, de trong de lay ngay he thong): ";
    getline(cin, ngayLap);
    if (ngayLap.empty()) {
        ngayLap = layNgayHeThong();
    }

    HoaDon hoaDonMoi(maHoaDon, maKhachHang, ngayLap, tongTien);
    hoaDonMoi.luuFile();

    cout << "Tong tien hoa don: " << fixed << setprecision(0) << tongTien << "\n";
}

void NhanVienBanHang::hienThiMenu() {
    int luaChon;
    do {
        cout << "\n===== MENU NHAN VIEN BAN HANG (" << getID() << ") =====\n";
        cout << "1. Tao tai khoan Khach hang\n";
        cout << "2. Lap hoa don ban hang\n";
        cout << "0. Dang xuat\n";
        luaChon = nhapSoNguyen("Lua chon: ");

        switch (luaChon) {
            case 1:
                TaoAccount("NV");
                break;
            case 2:
                LapHoaDonBanHang();
                break;
            case 0:
                cout << "Dang xuat...\n";
                break;
            default:
                cout << "Lua chon khong hop le!\n";
        }
    } while (luaChon != 0);
}