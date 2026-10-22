#include "NhanVienBanHang.h"
#include "Account.h"
#include "HoaDon.h"
#include "TienIch.h"
#include "KhoHang.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <set>

using namespace std;
namespace fs = std::filesystem;

NhanVienBanHang::NhanVienBanHang() : Nguoi() {}

NhanVienBanHang::NhanVienBanHang(string id, string mk) : Nguoi(id, mk) {}

NhanVienBanHang::~NhanVienBanHang() {}

// ------------------------------------------------------------
// Chuc nang: Lap hoa don ban hang voi giao dien Form phang can giua
// ------------------------------------------------------------
static void LapHoaDonBanHang(const string& idNhanVien) {
    clearScreen();
    veTieuDeForm("LẬP HÓA ĐƠN BÁN HÀNG",
                 "(Nhập thông tin khách hàng và Số Seri từng sản phẩm)",
                 62);

    string maHoaDon;
    string loi = "";
    while (true) {
        maHoaDon = nhapChuoiForm("Nhập Mã hóa đơn", loi, false, 62, 24);
        if (fs::exists("DataHoaDon/HD_" + maHoaDon + ".txt")) {
            xoaDongTruocForm(1);
            loi = "Mã hóa đơn này đã tồn tại! Vui lòng nhập mã khác.";
            continue;
        }
        break;
    }

    string maKhachHang;
    loi = "";
    while (true) {
        maKhachHang = nhapChuoiForm("Nhập Mã khách hàng", loi, false, 62, 24);
        if (!fs::exists("DataKhachHang/" + maKhachHang + ".txt")) {
            xoaDongTruocForm(1);
            loi = "Không tồn tại thông tin khách hàng này! Vui lòng nhập lại.";
            continue;
        }
        break;
    }

    double tongTien = 0;
    double tongDoanhThu = 0;
    string soSeri;
    set<string> daThemSoSeri;

    cout << "\n";
    inDongTrongKhungForm("NHẬP SỐ SERI MẶT HÀNG (Nhập 0 để kết thúc và chốt đơn)",
                         62, "\033[48;5;236m\033[93;1m");

    string loiSeri = "";
    do {
        soSeri = nhapChuoiForm("Số Seri", loiSeri, false, 62, 24);
        loiSeri = "";

        if (soSeri == "0") break;

        if (daThemSoSeri.count(soSeri) > 0) {
            xoaDongTruocForm(1);
            loiSeri = "Số seri này đã được thêm vào hóa đơn rồi!";
            continue;
        }

        string tenMatHang;
        double giaNhap, giaBan;
        bool daBanRoi;

        if (timMatHangTheoSoSeri(soSeri, tenMatHang, giaNhap, giaBan, daBanRoi)) {
            if (daBanRoi) {
                xoaDongTruocForm(1);
                loiSeri = "Sản phẩm (Seri: " + soSeri + ") đã được bán rồi!";
                continue;
            }

            tongTien += giaBan;
            tongDoanhThu += (giaBan - giaNhap);
            daThemSoSeri.insert(soSeri);

            inDongTrongKhungForm("+ Đã thêm: " + tenMatHang + " | Giá: " + dinhDangTien(giaBan) + " VNĐ",
                                 62, "\033[48;5;236m\033[92m");
        } else {
            xoaDongTruocForm(1);
            loiSeri = "Không tồn tại số seri '" + soSeri + "' trong kho!";
        }

    } while (soSeri != "0");

    if (tongTien <= 0) {
        cout << "\n";
        inThongBaoForm("Hóa đơn chưa có mặt hàng nào, hủy lập hóa đơn!", LOAI_LOI);
        return;
    }

    cout << "\n";
    string ngayLap = nhapChuoiForm("Ngày lập (trống = hôm nay)", "", true, 62, 24);
    if (ngayLap.empty()) {
        ngayLap = layNgayHeThong();
    }

    HoaDon hoaDonMoi(maHoaDon, maKhachHang, idNhanVien, ngayLap, tongTien, tongDoanhThu);
    hoaDonMoi.luuFile();

    for (const string& seri : daThemSoSeri) {
        danhDauMatHangDaBan(seri, maKhachHang);
    }

    cout << "\n";
    inTruongDaNhapForm("Tổng tiền hóa đơn", dinhDangTien(tongTien) + " VNĐ", 62, 24);
}

void NhanVienBanHang::hienThiMenu() {
    int luaChon;
    do {
        const vector<string> cacMuc = {
            "Tạo tài khoản Khách hàng",
            "Lập hóa đơn bán hàng",
            "Xuất danh sách kho hàng",
            "Tư vấn bán hàng (theo ngân sách khách)",
            "Đăng xuất"
        };
        int chiSo = chonMenu("MENU NHÂN VIÊN BÁN HÀNG (" + getID() + ")", cacMuc);
        luaChon = (chiSo == static_cast<int>(cacMuc.size()) - 1) ? 0 : chiSo + 1;

        switch (luaChon) {
            case 1:
                TaoAccount("NV");
                pauseScreen();
                break;
            case 2:
                LapHoaDonBanHang(getID());
                pauseScreen();
                break;
            case 3:
                XuatDanhSachKhoHang(false);
                pauseScreen();
                break;
            case 4:
                TuVanBanHang();
                pauseScreen();
                break;
            case 0:
                break;
            default:
                break;
        }
    } while (luaChon != 0);
}
