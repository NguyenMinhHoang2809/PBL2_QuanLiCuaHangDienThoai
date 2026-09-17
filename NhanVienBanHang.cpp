#include "NhanVienBanHang.h"
#include "Account.h"        // TaoAccount()
#include "HoaDon.h"         // HoaDon, luuFile()
#include "Utils.h"          // layNgayHeThong(), timMatHangTheoSoSeri()
#include "KhoHang.h"        // XuatDanhSachKhoHang(), TuVanBanHang()
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
static void LapHoaDonBanHang(const string& idNhanVien) {
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
    // Lap lai cho den khi nhap duoc Ma Khach Hang CO THAT SU TON TAI
    // trong DataKhachHang/ - tranh truong hop go nham/go bua ID khach
    // hang, khien hoa don duoc lap cho mot khach hang khong co thuc,
    // sau nay khach hang do khong the dang nhap de xem lai hoa don nay.
    while (true) {
        cout << "Nhap Ma Khach Hang: ";
        cin >> maKhachHang;

        if (!fs::exists("DataKhachHang/" + maKhachHang + ".txt")) {
            cout << "Khong ton tai thong tin khach hang nay! Vui long nhap lai.\n";
            continue;
        }
        break;
    }

    double tongTien = 0;
    // Doanh Thu (theo dung nghia ban dua ra: Gia Ban - Gia Nhap cong don
    // tung san pham) - ve ban chat day la LOI NHUAN GOP, khac voi doanh
    // thu ke toan chuan (= tongTien). Ten bien/truong van dat la "doanh
    // thu" de khop thuat ngu ban su dung trong yeu cau.
    double tongDoanhThu = 0;
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
        double giaNhap, giaBan;
        bool daBanRoi;

        if (timMatHangTheoSoSeri(soSeri, tenMatHang, giaNhap, giaBan, daBanRoi)) {
            if (daBanRoi) {
                cout << "San pham nay (Seri: " << soSeri << ") da duoc ban roi, khong the ban lai!\n";
                continue;
            }

            tongTien += giaBan;
            tongDoanhThu += (giaBan - giaNhap);
            daThemSoSeri.insert(soSeri); // Danh dau seri nay DA duoc them
            cout << "Da them " << tenMatHang << " - Gia: "
                 << fixed << setprecision(0) << giaBan
                 << " (Loi nhuan: " << (giaBan - giaNhap) << ")\n";
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

    HoaDon hoaDonMoi(maHoaDon, maKhachHang, idNhanVien, ngayLap, tongTien, tongDoanhThu);
    hoaDonMoi.luuFile();

    // Chi sau khi hoa don da LUU FILE THANH CONG moi danh dau cac san
    // pham la "Da Ban" - tranh truong hop luuFile() that bai (vd: khong
    // mo duoc file) ma san pham van bi danh dau ban nham.
    for (const string& seri : daThemSoSeri) {
        danhDauMatHangDaBan(seri, maKhachHang);
    }

    cout << "Tong tien hoa don: " << fixed << setprecision(0) << tongTien << "\n";
}

void NhanVienBanHang::hienThiMenu() {
    int luaChon;
    do {
        cout << "\n===== MENU NHAN VIEN BAN HANG (" << getID() << ") =====\n";
        cout << "1. Tao tai khoan Khach hang\n";
        cout << "2. Lap hoa don ban hang\n";
        cout << "3. Xuat danh sach kho hang\n";
        cout << "4. Tu van ban hang (theo ngan sach khach)\n";
        cout << "0. Dang xuat\n";
        luaChon = nhapSoNguyen("Lua chon: ");

        switch (luaChon) {
            case 1:
                TaoAccount("NV");
                break;
            case 2:
                LapHoaDonBanHang(getID());
                break;
            case 3:
                // false = Nhan Vien KHONG duoc xem Gia Nhap (thong tin
                // kinh doanh noi bo - biet gia nhap la biet bien loi nhuan)
                XuatDanhSachKhoHang(false);
                break;
            case 4:
                TuVanBanHang();
                break;
            case 0:
                cout << "Dang xuat...\n";
                break;
            default:
                cout << "Lua chon khong hop le!\n";
        }
    } while (luaChon != 0);
}