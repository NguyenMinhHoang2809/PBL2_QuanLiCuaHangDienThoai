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
#include <vector>

using namespace std;
namespace fs = std::filesystem;

NhanVienBanHang::NhanVienBanHang() : Nguoi() {}

NhanVienBanHang::NhanVienBanHang(string id, string mk, string hoTen) : Nguoi(id, mk, hoTen) {}

NhanVienBanHang::~NhanVienBanHang() {}

static string canTraiUTF8(const string& s, int rongCot) {
    int hienThi = doRongHienThi(s);
    if (hienThi >= rongCot) return s;
    return s + string(rongCot - hienThi, ' ');
}

static string catKhoangTrangDauCuoi(const string& s) {
    size_t dau = s.find_first_not_of(" \t\r\n");
    if (dau == string::npos) return "";
    size_t cuoi = s.find_last_not_of(" \t\r\n");
    return s.substr(dau, cuoi - dau + 1);
}

// ------------------------------------------------------------
// Chuc nang: Tim kiem thong tin Khach Hang & Lich su mua hang
// ------------------------------------------------------------
static void TimKiemThongTinKhachHang() {
    clearScreen();
    int rongBang = 82;
    veTieuDeForm("TÌM KIẾM THÔNG TIN KHÁCH HÀNG",
                 "(Nhập ID Khách hàng cần tra cứu, ví dụ: KH_01)",
                 rongBang);

    string maKhachHang = nhapChuoiForm("ID Khách hàng", "", false, 58, 20);

    string matKhau, hoTen;
    if (!docThongTinTaiKhoan("DataKhachHang", maKhachHang, matKhau, hoTen)) {
        cout << "\n";
        inThongBaoForm("Không tìm thấy khách hàng với ID này!", LOAI_LOI);
        return;
    }

    cout << "\n";
    inDongTrongKhungForm("THÔNG TIN KHÁCH HÀNG", rongBang, "\033[48;5;236m\033[93;1m");
    inTruongDaNhapForm("ID Khách hàng", maKhachHang, rongBang, 20);
    inTruongDaNhapForm("Họ và tên",     hoTen,       rongBang, 20);

    // Tra cuu toan bo hoa don cua khach hang trong DataHoaDon
    struct HoaDonKhachHang {
        string maHD;
        string ngayLap;
        string maNV;
        string soLuongStr;
        double tongTien = 0;
        vector<string> dsSanPham;
    };

    vector<HoaDonKhachHang> lichSuHoaDon;
    string thuMucHoaDon = "DataHoaDon";

    if (fs::exists(thuMucHoaDon) && fs::is_directory(thuMucHoaDon)) {
        for (const auto& entry : fs::directory_iterator(thuMucHoaDon)) {
            if (!entry.is_regular_file()) continue;

            ifstream fileDoc(entry.path());
            if (!fileDoc.is_open()) continue;

            HoaDonKhachHang hd;
            hd.maHD = "(không rõ)";
            hd.ngayLap = "(không rõ)";
            hd.maNV = "(không rõ)";
            string maKHTrongFile = "", tongTienStr = "0";

            string dong;
            while (getline(fileDoc, dong)) {
                if (dong.rfind("Ma Hoa Don:", 0) == 0) {
                    hd.maHD = catKhoangTrangDauCuoi(dong.substr(dong.find(":") + 1));
                } else if (dong.rfind("Ma Khach Hang:", 0) == 0) {
                    maKHTrongFile = catKhoangTrangDauCuoi(dong.substr(dong.find(":") + 1));
                } else if (dong.rfind("Ma Nhan Vien:", 0) == 0) {
                    hd.maNV = catKhoangTrangDauCuoi(dong.substr(dong.find(":") + 1));
                } else if (dong.rfind("Ngay Lap:", 0) == 0) {
                    hd.ngayLap = catKhoangTrangDauCuoi(dong.substr(dong.find(":") + 1));
                } else if (dong.rfind("So Luong:", 0) == 0) {
                    hd.soLuongStr = catKhoangTrangDauCuoi(dong.substr(dong.find(":") + 1));
                } else if (dong.rfind("Tong Tien:", 0) == 0) {
                    tongTienStr = catKhoangTrangDauCuoi(dong.substr(dong.find(":") + 1));
                } else if (dong.rfind("San Pham:", 0) == 0) {
                    hd.dsSanPham.push_back(catKhoangTrangDauCuoi(dong.substr(dong.find(":") + 1)));
                }
            }
            fileDoc.close();

            if (maKHTrongFile == maKhachHang) {
                try { hd.tongTien = stod(tongTienStr); } catch (...) { hd.tongTien = 0; }
                if (hd.soLuongStr.empty()) {
                    hd.soLuongStr = hd.dsSanPham.empty() ? "1" : to_string(hd.dsSanPham.size());
                }
                lichSuHoaDon.push_back(hd);
            }
        }
    }

    // Lay them danh sach san pham da mua tu DataMatHang (cho cac hoa don cu chua ghi dong San Pham:)
    vector<ThongTinMatHang> tatCaKho = docToanBoKhoHang();
    vector<ThongTinMatHang> spDaMuaTrongKho;
    for (const auto& mh : tatCaKho) {
        if (mh.daBan && mh.maKhachHangDaMua == maKhachHang) {
            spDaMuaTrongKho.push_back(mh);
        }
    }

    cout << "\n";
    inDongTrongKhungForm("LỊCH SỬ MUA HÀNG CỦA KHÁCH HÀNG", rongBang, "\033[48;5;236m\033[93;1m");

    if (lichSuHoaDon.empty() && spDaMuaTrongKho.empty()) {
        cout << "\n";
        inThongBaoForm("Chưa có lịch sử mua hàng", LOAI_GHI_CHU);
        return;
    }

    int leTrai = tinhLeTraiCanGiua(rongBang);
    string khoangLe(leTrai, ' ');

    double tongChiTieu = 0;
    bool coChiTietSanPhamTrongHD = false;

    if (!lichSuHoaDon.empty()) {
        string tieuDeCot = "  " + canTraiUTF8("Mã Hóa đơn", 15)
                         + canTraiUTF8("Ngày mua", 15)
                         + canTraiUTF8("Nhân viên bán", 16)
                         + canTraiUTF8("Số lượng", 12)
                         + canTraiUTF8("Tổng tiền (VNĐ)", 22);
        cout << khoangLe << "\033[48;5;236m\033[93;1m" << canTraiUTF8(tieuDeCot, rongBang) << "\033[0m\n";

        int stt = 0;
        for (const auto& hd : lichSuHoaDon) {
            stt++;
            tongChiTieu += hd.tongTien;
            const char* mauNen = (stt % 2 == 1) ? "\033[48;5;235m\033[97m" : "\033[48;5;237m\033[97m";
            string dongIn = "  " + canTraiUTF8(hd.maHD, 15)
                          + canTraiUTF8(hd.ngayLap, 15)
                          + canTraiUTF8(hd.maNV, 16)
                          + canTraiUTF8(hd.soLuongStr, 12)
                          + canTraiUTF8(dinhDangTien(hd.tongTien), 22);
            cout << khoangLe << mauNen << canTraiUTF8(dongIn, rongBang) << "\033[0m\n";

            for (const string& sp : hd.dsSanPham) {
                coChiTietSanPhamTrongHD = true;
                inDongTrongKhungForm("   • Sản phẩm: " + sp, rongBang, "\033[48;5;236m\033[92m");
            }
        }
    }

    // Hien thi danh sach san pham da mua (lay tu DataMatHang) neu hoa don cu chua co chi tiet San Pham
    if (!coChiTietSanPhamTrongHD && !spDaMuaTrongKho.empty()) {
        cout << "\n";
        inDongTrongKhungForm("DANH SÁCH SẢN PHẨM ĐÃ MUA", rongBang, "\033[48;5;236m\033[93;1m");
        for (const auto& sp : spDaMuaTrongKho) {
            inDongTrongKhungForm("  • " + sp.tenMatHang + "  |  Số Seri: " + sp.soSeri
                                 + "  |  Thành tiền: " + dinhDangTien(sp.giaBan) + " VNĐ",
                                 rongBang, "\033[48;5;236m\033[92m");
        }
    }

    cout << "\n";
    inTruongDaNhapForm("Tổng số hóa đơn", to_string(lichSuHoaDon.size()) + " hóa đơn", rongBang, 20);
    inTruongDaNhapForm("Tổng thành tiền", dinhDangTien(tongChiTieu) + " VNĐ", rongBang, 20);
}

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
    vector<string> dsSanPhamMua;

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
            dsSanPhamMua.push_back(tenMatHang + " (Seri: " + soSeri + ") - " + dinhDangTien(giaBan) + " VNĐ");

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

    HoaDon hoaDonMoi(maHoaDon, maKhachHang, idNhanVien, ngayLap, tongTien, tongDoanhThu, dsSanPhamMua);
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
            "Tìm kiếm thông tin Khách hàng",
            "Lập hóa đơn bán hàng",
            "Xuất danh sách kho hàng",
            "Tư vấn bán hàng (theo ngân sách khách)",
            "Đăng xuất"
        };
        int chiSo = chonMenu("MENU NHÂN VIÊN BÁN HÀNG (" + getID() + ")", cacMuc);
        luaChon = (chiSo == static_cast<int>(cacMuc.size()) - 1) ? 0 : chiSo + 1;

        // ESC / click nut "Quay lai" o bat ky man hinh chuc nang nao se nem
        // HuyThaoTac -> bat lai o day va hien lai menu Nhan vien.
        try {
            switch (luaChon) {
                case 1:
                    TaoAccount("NV");
                    pauseScreen();
                    break;
                case 2:
                    TimKiemThongTinKhachHang();
                    pauseScreen();
                    break;
                case 3:
                    LapHoaDonBanHang(getID());
                    pauseScreen();
                    break;
                case 4:
                    XuatDanhSachKhoHang(false);
                    pauseScreen();
                    break;
                case 5:
                    TuVanBanHang();
                    pauseScreen();
                    break;
                case 0:
                    break;
                default:
                    break;
            }
        } catch (const HuyThaoTac&) {
            // Huy thao tac -> quay lai menu
        }
    } while (luaChon != 0);
}
