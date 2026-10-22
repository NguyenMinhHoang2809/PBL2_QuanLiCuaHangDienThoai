#include "ChuCuaHang.h"
#include "NhapHangHoa.h"
#include "Account.h"
#include "TienIch.h"
#include "KhoHang.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <map>
#include <sstream>

using namespace std;
namespace fs = std::filesystem;

ChuCuaHang::ChuCuaHang() : Nguoi() {}

ChuCuaHang::ChuCuaHang(string id, string mk) : Nguoi(id, mk) {}

ChuCuaHang::~ChuCuaHang() {}

static string canTraiUTF8(const string& s, int rongCot) {
    int hienThi = doRongHienThi(s);
    if (hienThi >= rongCot) return s;
    return s + string(rongCot - hienThi, ' ');
}

// ------------------------------------------------------------
// phanTichNgay(): tach chuoi "dd-mm-yyyy" thanh 3 so nguyen ngay/thang/nam.
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

struct ThongTinHoaDon {
    string maHD, maKH, maNV, ngayLap;
    string tongTienStr, doanhThuStr;
    double tongTien = 0;
    double doanhThu = 0;
};

static string boKhoangTrangDau(const string& s) {
    size_t vt = s.find_first_not_of(" \t");
    return (vt == string::npos) ? "" : s.substr(vt);
}

static bool docHoaDon(const fs::path& duongDan, ThongTinHoaDon& hd) {
    ifstream fileDoc(duongDan);
    if (!fileDoc.is_open()) return false;

    string dong;
    while (getline(fileDoc, dong)) {
        size_t viTriHaiCham = dong.find(":");
        if (viTriHaiCham == string::npos) continue;
        string giaTri = boKhoangTrangDau(dong.substr(viTriHaiCham + 1));

        if (dong.rfind("Ma Hoa Don:", 0) == 0)          hd.maHD = giaTri;
        else if (dong.rfind("Ma Khach Hang:", 0) == 0)  hd.maKH = giaTri;
        else if (dong.rfind("Ma Nhan Vien:", 0) == 0)   hd.maNV = giaTri;
        else if (dong.rfind("Ngay Lap:", 0) == 0)       hd.ngayLap = giaTri;
        else if (dong.rfind("Tong Tien:", 0) == 0)      hd.tongTienStr = giaTri;
        else if (dong.rfind("Doanh Thu:", 0) == 0)      hd.doanhThuStr = giaTri;
    }
    fileDoc.close();

    try { if (!hd.tongTienStr.empty()) hd.tongTien = stod(hd.tongTienStr); } catch (...) { hd.tongTien = 0; }
    try { if (!hd.doanhThuStr.empty()) hd.doanhThu = stod(hd.doanhThuStr); } catch (...) { hd.doanhThu = 0; }
    return true;
}

static bool khopThoiGian(const string& ngayLap, int locTheo, int ngayLoc, int thangLoc, int namLoc) {
    if (locTheo != 1 && locTheo != 2 && locTheo != 3) return true;

    int ngay, thang, nam;
    if (!phanTichNgay(ngayLap, ngay, thang, nam)) return false;

    if (locTheo == 1) return ngay == ngayLoc && thang == thangLoc && nam == namLoc;
    if (locTheo == 2) return thang == thangLoc && nam == namLoc;
    return nam == namLoc;
}

static int nhapSoTrongKhoang(const string& thongBao, int nhoNhat, int lonNhat) {
    string loi = "";
    while (true) {
        string chuoi = nhapChuoiForm(thongBao, loi, false, 58, 22);
        try {
            size_t viTri = 0;
            int giaTri = stoi(chuoi, &viTri);
            if (viTri == chuoi.size()) {
                if (giaTri >= nhoNhat && giaTri <= lonNhat) return giaTri;
                xoaDongTruocForm(1);
                loi = "Giá trị phải nằm trong khoảng " + to_string(nhoNhat) + " - " + to_string(lonNhat) + "!";
                continue;
            }
        } catch (...) {}

        xoaDongTruocForm(1);
        loi = "Dữ liệu không hợp lệ! Vui lòng nhập số nguyên.";
    }
}

// ------------------------------------------------------------
// Chuc nang: Xem doanh thu / danh sach hoa don cua 1 Nhan Vien cu the
// ------------------------------------------------------------
static void XemDoanhThuNhanVien() {
    clearScreen();
    veTieuDeForm("DOANH THU & HÓA ĐƠN THEO NHÂN VIÊN",
                 "(Nhập Mã nhân viên cần tra cứu, ví dụ: NV_01)",
                 58);

    string maNhanVien;
    string loi = "";
    while (true) {
        maNhanVien = nhapChuoiForm("Nhập Mã nhân viên", loi, false, 58, 22);
        if (!fs::exists("DataNhanVien/" + maNhanVien + ".txt")) {
            xoaDongTruocForm(1);
            loi = "Không tồn tại tài khoản Nhân viên này! Vui lòng nhập lại.";
            continue;
        }
        break;
    }

    const vector<string> cacMucLoc = {
        "Ngày cụ thể (dd-mm-yyyy)",
        "Tháng (mm-yyyy)",
        "Năm (yyyy)",
        "Tất cả (không lọc thời gian)",
        "Quay lại"
    };
    int chiSoLoc = chonMenu("LỌC BÁO CÁO CỦA " + maNhanVien + " THEO", cacMucLoc);
    if (chiSoLoc == static_cast<int>(cacMucLoc.size()) - 1) return;
    int locTheo = chiSoLoc + 1;

    clearScreen();
    veTieuDeForm("HÓA ĐƠN CỦA NHÂN VIÊN " + maNhanVien, "", 78);

    int ngayLoc = 0, thangLoc = 0, namLoc = 0;
    if (locTheo == 1) {
        ngayLoc  = nhapSoTrongKhoang("Nhập ngày (dd)", 1, 31);
        thangLoc = nhapSoTrongKhoang("Nhập tháng (mm)", 1, 12);
        namLoc   = nhapSoTrongKhoang("Nhập năm (yyyy)", 1900, 9999);
        cout << "\n";
    } else if (locTheo == 2) {
        thangLoc = nhapSoTrongKhoang("Nhập tháng (mm)", 1, 12);
        namLoc   = nhapSoTrongKhoang("Nhập năm (yyyy)", 1900, 9999);
        cout << "\n";
    } else if (locTheo == 3) {
        namLoc   = nhapSoTrongKhoang("Nhập năm (yyyy)", 1900, 9999);
        cout << "\n";
    }

    string thuMucHoaDon = "DataHoaDon";
    double tongDoanhThu = 0;
    double tongDoanhSo = 0;
    int soLuongHoaDon = 0;

    int rongBang = 78;
    int leTrai = tinhLeTraiCanGiua(rongBang);
    string khoangLe(leTrai, ' ');

    string tieuDeCot = "  " + canTraiUTF8("Mã HĐ", 12)
                     + canTraiUTF8("Khách hàng", 14)
                     + canTraiUTF8("Ngày lập", 14)
                     + canTraiUTF8("Tổng tiền (VNĐ)", 18)
                     + canTraiUTF8("Doanh thu (VNĐ)", 18);
    cout << khoangLe << "\033[48;5;236m\033[93;1m" << canTraiUTF8(tieuDeCot, rongBang) << "\033[0m\n";

    if (fs::exists(thuMucHoaDon) && fs::is_directory(thuMucHoaDon)) {
        for (const auto& entry : fs::directory_iterator(thuMucHoaDon)) {
            if (!entry.is_regular_file()) continue;

            ThongTinHoaDon hd;
            if (!docHoaDon(entry.path(), hd)) continue;
            if (hd.maNV != maNhanVien) continue;
            if (!khopThoiGian(hd.ngayLap, locTheo, ngayLoc, thangLoc, namLoc)) continue;

            soLuongHoaDon++;
            const char* mauNen = (soLuongHoaDon % 2 == 1) ? "\033[48;5;235m\033[97m" : "\033[48;5;237m\033[97m";
            string dong = "  " + canTraiUTF8(hd.maHD, 12)
                        + canTraiUTF8(hd.maKH, 14)
                        + canTraiUTF8(hd.ngayLap, 14)
                        + canTraiUTF8(dinhDangTien(hd.tongTien), 18)
                        + canTraiUTF8(dinhDangTien(hd.doanhThu), 18);
            cout << khoangLe << mauNen << canTraiUTF8(dong, rongBang) << "\033[0m\n";

            tongDoanhSo += hd.tongTien;
            tongDoanhThu += hd.doanhThu;
        }
    }

    if (soLuongHoaDon == 0) {
        cout << "\n";
        inThongBaoForm("Không tìm thấy hóa đơn nào phù hợp.", LOAI_GHI_CHU);
        return;
    }

    cout << "\n";
    inTruongDaNhapForm("Số lượng hóa đơn", to_string(soLuongHoaDon) + " hóa đơn", 78, 22);
    inTruongDaNhapForm("Tổng doanh số",    dinhDangTien(tongDoanhSo) + " VNĐ (tổng Giá bán)", 78, 22);
    inTruongDaNhapForm("Tổng doanh thu",   dinhDangTien(tongDoanhThu) + " VNĐ (tổng Giá bán - Giá nhập)", 78, 22);
}

// ------------------------------------------------------------
// Chuc nang: Xem doanh thu TONG HOP cua TAT CA Nhan Vien
// ------------------------------------------------------------
static void XemDoanhThuTatCaNhanVien(bool theoNgay) {
    clearScreen();
    veTieuDeForm(theoNgay ? "DOANH THU TẤT CẢ NHÂN VIÊN THEO NGÀY"
                          : "DOANH THU TẤT CẢ NHÂN VIÊN THEO THÁNG",
                 "(Nhập thời gian cần thống kê doanh thu)",
                 72);

    int ngayLoc = 0;
    if (theoNgay) ngayLoc = nhapSoTrongKhoang("Nhập ngày (dd)", 1, 31);
    int thangLoc = nhapSoTrongKhoang("Nhập tháng (mm)", 1, 12);
    int namLoc   = nhapSoTrongKhoang("Nhập năm (yyyy)", 1900, 9999);
    int locTheo  = theoNgay ? 1 : 2;

    struct TongKetNhanVien {
        int soHoaDon = 0;
        double doanhSo = 0;
        double doanhThu = 0;
    };
    map<string, TongKetNhanVien> bangTongKet;

    string thuMucNhanVien = "DataNhanVien";
    if (fs::exists(thuMucNhanVien) && fs::is_directory(thuMucNhanVien)) {
        for (const auto& entry : fs::directory_iterator(thuMucNhanVien)) {
            if (!entry.is_regular_file() || entry.path().extension() != ".txt") continue;
            bangTongKet[entry.path().stem().string()];
        }
    }

    string thuMucHoaDon = "DataHoaDon";
    if (fs::exists(thuMucHoaDon) && fs::is_directory(thuMucHoaDon)) {
        for (const auto& entry : fs::directory_iterator(thuMucHoaDon)) {
            if (!entry.is_regular_file()) continue;

            ThongTinHoaDon hd;
            if (!docHoaDon(entry.path(), hd)) continue;
            if (hd.maNV.empty()) continue;
            if (!khopThoiGian(hd.ngayLap, locTheo, ngayLoc, thangLoc, namLoc)) continue;

            TongKetNhanVien& tk = bangTongKet[hd.maNV];
            tk.soHoaDon++;
            tk.doanhSo += hd.tongTien;
            tk.doanhThu += hd.doanhThu;
        }
    }

    ostringstream kyBaoCao;
    kyBaoCao << setfill('0');
    if (theoNgay) kyBaoCao << setw(2) << ngayLoc << "-";
    kyBaoCao << setw(2) << thangLoc << "-" << setw(4) << namLoc;

    cout << "\n";
    inDongCanGiua(string("KỲ BÁO CÁO: ") + (theoNgay ? "NGÀY " : "THÁNG ") + kyBaoCao.str(), "\033[93;1m");
    cout << "\n";

    if (bangTongKet.empty()) {
        inThongBaoForm("Chưa có nhân viên / hóa đơn nào trong hệ thống.", LOAI_GHI_CHU);
        return;
    }

    int rongBang = 72;
    int leTrai = tinhLeTraiCanGiua(rongBang);
    string khoangLe(leTrai, ' ');

    string tieuDeCot = "  " + canTraiUTF8("Mã Nhân viên", 16)
                     + canTraiUTF8("Số Hóa đơn", 14)
                     + canTraiUTF8("Doanh số (VNĐ)", 20)
                     + canTraiUTF8("Doanh thu (VNĐ)", 20);
    cout << khoangLe << "\033[48;5;236m\033[93;1m" << canTraiUTF8(tieuDeCot, rongBang) << "\033[0m\n";

    int tongSoHoaDon = 0, demDong = 0;
    double tongDoanhSo = 0, tongDoanhThu = 0;
    for (const auto& cap : bangTongKet) {
        demDong++;
        const TongKetNhanVien& tk = cap.second;
        const char* mauNen = (demDong % 2 == 1) ? "\033[48;5;235m\033[97m" : "\033[48;5;237m\033[97m";
        string dong = "  " + canTraiUTF8(cap.first, 16)
                    + canTraiUTF8(to_string(tk.soHoaDon), 14)
                    + canTraiUTF8(dinhDangTien(tk.doanhSo), 20)
                    + canTraiUTF8(dinhDangTien(tk.doanhThu), 20);
        cout << khoangLe << mauNen << canTraiUTF8(dong, rongBang) << "\033[0m\n";

        tongSoHoaDon += tk.soHoaDon;
        tongDoanhSo += tk.doanhSo;
        tongDoanhThu += tk.doanhThu;
    }

    string dongTong = "  " + canTraiUTF8("TỔNG CỘNG", 16)
                    + canTraiUTF8(to_string(tongSoHoaDon), 14)
                    + canTraiUTF8(dinhDangTien(tongDoanhSo), 20)
                    + canTraiUTF8(dinhDangTien(tongDoanhThu), 20);
    cout << khoangLe << "\033[48;5;236m\033[92;1m" << canTraiUTF8(dongTong, rongBang) << "\033[0m\n\n";

    inThongBaoForm("Doanh số = tổng Giá bán  |  Doanh thu = tổng (Giá bán - Giá nhập)", LOAI_GHI_CHU);
    if (tongSoHoaDon == 0) {
        inThongBaoForm("Không có hóa đơn nào được lập trong kỳ này.", LOAI_GHI_CHU);
    }
}

// ------------------------------------------------------------
// Chuc nang: "Xoa" (vo hieu hoa) tai khoan Nhan Vien
// ------------------------------------------------------------
static void XoaTaiKhoanNhanVien() {
    clearScreen();
    veTieuDeForm("VÔ HIỆU HÓA TÀI KHOẢN NHÂN VIÊN",
                 "(Đặt lại mật khẩu về mặc định để khóa quyền đăng nhập)",
                 58);

    string maNhanVien;
    string loi = "";
    while (true) {
        maNhanVien = nhapChuoiForm("Mã nhân viên cần khóa", loi, false, 58, 24);
        if (!fs::exists("DataNhanVien/" + maNhanVien + ".txt")) {
            xoaDongTruocForm(1);
            loi = "Không tồn tại tài khoản Nhân viên này! Vui lòng nhập lại.";
            continue;
        }
        break;
    }

    string tenFile = "DataNhanVien/" + maNhanVien + ".txt";
    ofstream file(tenFile);
    if (!file.is_open()) {
        cout << "\n";
        inThongBaoForm("Không thể cập nhật tài khoản này!", LOAI_LOI);
        return;
    }

    file << maNhanVien << "\n31415\n";
    file.close();

    cout << "\n";
    inThongBaoForm("Đã vô hiệu hóa tài khoản " + maNhanVien + " thành công!", LOAI_THANH_CONG);
}

void ChuCuaHang::hienThiMenu() {
    int luaChon;
    do {
        const vector<string> cacMuc = {
            "Nhập hàng hóa mới (1 thiết bị)",
            "Nhập hàng LOẠT từ file",
            "Xuất danh sách kho hàng",
            "Tạo tài khoản Nhân viên",
            "Xem doanh thu / hóa đơn theo Nhân viên",
            "Xem doanh thu TẤT CẢ Nhân viên theo Ngày",
            "Xem doanh thu TẤT CẢ Nhân viên theo Tháng",
            "Xóa (vô hiệu hóa) tài khoản Nhân viên",
            "Đăng xuất"
        };
        int chiSo = chonMenu("MENU CHỦ CỬA HÀNG (" + getID() + ")", cacMuc);
        luaChon = (chiSo == static_cast<int>(cacMuc.size()) - 1) ? 0 : chiSo + 1;

        switch (luaChon) {
            case 1:
                TaoMatHang();
                pauseScreen();
                break;
            case 2:
                NhapHangLoatTuFile();
                pauseScreen();
                break;
            case 3:
                XuatDanhSachKhoHang(true);
                pauseScreen();
                break;
            case 4:
                TaoAccount("QL");
                pauseScreen();
                break;
            case 5:
                XemDoanhThuNhanVien();
                pauseScreen();
                break;
            case 6:
                XemDoanhThuTatCaNhanVien(true);
                pauseScreen();
                break;
            case 7:
                XemDoanhThuTatCaNhanVien(false);
                pauseScreen();
                break;
            case 8:
                XoaTaiKhoanNhanVien();
                pauseScreen();
                break;
            case 0:
                break;
            default:
                break;
        }
    } while (luaChon != 0);
}
