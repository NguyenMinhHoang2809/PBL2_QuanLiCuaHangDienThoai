#include "Account.h"
#include "TienIch.h"
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

static void xoaKyTuCuoiDong(string& s) {
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n')) {
        s.pop_back();
    }
}

// ------------------------------------------------------------
// docThongTinTaiKhoan: doc file tai khoan (Dong 1: ID, Dong 2: MatKhau,
// Dong 3: HoTen). Tuong thich nguoc 100% voi file cu chi co 2 dong.
// ------------------------------------------------------------
bool docThongTinTaiKhoan(const string& thuMuc, const string& id,
                         string& matKhauRa, string& hoTenRa) {
    string tenFile = thuMuc + "/" + id + ".txt";
    if (!fs::exists(tenFile)) return false;

    ifstream file(tenFile);
    if (!file.is_open()) return false;

    string idTrongFile;
    getline(file, idTrongFile);
    getline(file, matKhauRa);
    if (!getline(file, hoTenRa)) {
        hoTenRa = "";
    }
    file.close();

    xoaKyTuCuoiDong(matKhauRa);
    xoaKyTuCuoiDong(hoTenRa);

    // Cat khoang trang 2 dau cua hoTenRa
    size_t dau = hoTenRa.find_first_not_of(" \t");
    if (dau == string::npos) {
        hoTenRa = "(Chưa cập nhật)";
    } else {
        size_t cuoi = hoTenRa.find_last_not_of(" \t");
        hoTenRa = hoTenRa.substr(dau, cuoi - dau + 1);
    }
    return true;
}

string layHoTenTaiKhoan(const string& thuMuc, const string& id) {
    string mk, hoTen;
    if (docThongTinTaiKhoan(thuMuc, id, mk, hoTen)) {
        return hoTen;
    }
    return "(Chưa cập nhật)";
}

// ------------------------------------------------------------
// TaoAccount: hien thi khung Form TAO TAI KHOAN phang can giua man hinh
//   - QL : chi duoc tao tai khoan Quan ly (QL_...) hoac Nhan vien (NV_...)
//   - NV : chi duoc tao tai khoan Khach hang (KH_...)
//   - Neu ID da ton tai va DANG HOAT DONG -> bao loi "ID nay da ton tai!"
//   - Neu ID da ton tai nhung DA BI XOA (__DELETED_ACCOUNT__) -> cho
//     phep nhap Ho ten moi + Mat khau moi va ghi de truc tiep vao file
//     cua ID do (khoi phuc tai khoan, giu nguyen lich su hoa don cu).
//   - Chan khong cho dat mat khau trung voi "__DELETED_ACCOUNT__".
// ------------------------------------------------------------
void TaoAccount(string vaiTroNguoiTao) {
    clearScreen();
    string ghiChu = (vaiTroNguoiTao == "NV")
        ? "(Nhân viên tạo tài khoản Khách hàng - bắt đầu bằng KH_)"
        : "(Định dạng ID: QL_... hoặc NV_...)";
    veTieuDeForm("TẠO TÀI KHOẢN", ghiChu, 56);

    if (vaiTroNguoiTao == "KH") {
        inThongBaoForm("Khách hàng không có quyền tạo tài khoản!", LOAI_LOI);
        return;
    }

    string ID, HoTen, MatKhau, ThuMuc;
    ID = nhapChuoiForm("Nhập ID", "", false, 56, 16);

    string tienTo = (ID.size() >= 3) ? ID.substr(0, 3) : "";

    if (vaiTroNguoiTao == "QL") {
        if (tienTo == "QL_") {
            ThuMuc = "DataQuanLi";
        } else if (tienTo == "NV_") {
            ThuMuc = "DataNhanVien";
        } else if (tienTo == "KH_") {
            cout << "\n";
            inThongBaoForm("ID không hợp lệ! Chủ cửa hàng chỉ được tạo tài khoản QL_... hoặc NV_...", LOAI_LOI);
            return;
        } else {
            cout << "\n";
            inThongBaoForm("ID không hợp lệ! (phải bắt đầu bằng QL_ hoặc NV_)", LOAI_LOI);
            return;
        }
    } else if (vaiTroNguoiTao == "NV") {
        if (tienTo == "KH_") {
            ThuMuc = "DataKhachHang";
        } else {
            cout << "\n";
            inThongBaoForm("Nhân viên chỉ được tạo tài khoản Khách hàng (bắt đầu bằng KH_)!", LOAI_LOI);
            return;
        }
    } else {
        cout << "\n";
        inThongBaoForm("Vai trò không hợp lệ!", LOAI_LOI);
        return;
    }

    fs::create_directories(ThuMuc);
    string TenFile = ThuMuc + "/" + ID + ".txt";

    bool laKhoiPhucTaiKhoanDaXoa = false;
    if (fs::exists(TenFile)) {
        string mkHienTai, hoTenHienTai;
        if (docThongTinTaiKhoan(ThuMuc, ID, mkHienTai, hoTenHienTai)
            && mkHienTai == MAT_KHAU_DA_XOA) {
            // Tai khoan da bi xoa/vo hieu hoa -> cho phep tao lai (khoi phuc)
            laKhoiPhucTaiKhoanDaXoa = true;
        } else {
            // Tai khoan dang hoat dong -> chan khong cho tao trung
            cout << "\n";
            inThongBaoForm("ID này đã tồn tại!", LOAI_LOI);
            return;
        }
    }

    HoTen = nhapChuoiForm("Họ và tên", "", false, 56, 16);

    string loiMatKhau = "";
    while (true) {
        MatKhau = nhapChuoiForm("Mật khẩu", loiMatKhau, false, 56, 16);
        if (MatKhau == MAT_KHAU_DA_XOA) {
            xoaDongTruocForm(1);
            loiMatKhau = "Mật khẩu này không hợp lệ, vui lòng chọn mật khẩu khác!";
            continue;
        }
        break;
    }

    // Ghi de truc tiep vao file cua ID do (ios::trunc) - khong ghi noi
    // them dong trung ID vao cuoi file, giu nguyen lich su hoa don cu.
    ofstream file(TenFile, ios::trunc);
    file << ID << "\n" << MatKhau << "\n" << HoTen << "\n";
    file.close();

    cout << "\n";
    if (laKhoiPhucTaiKhoanDaXoa) {
        inThongBaoForm("Đã tạo lại (khôi phục) tài khoản thành công!", LOAI_THANH_CONG);
    } else {
        inThongBaoForm("Đã tạo tài khoản thành công!", LOAI_THANH_CONG);
    }
}

// ------------------------------------------------------------
// DangNhap: giao dien FORM DANG NHAP PHANG (Flat UI) can giua man hinh:
//   - ID sai dinh dang / khong ton tai -> xoa ID, nhap lai ID.
//   - Tai khoan co mat khau la "__DELETED_ACCOUNT__" -> chan tuyet doi
//     (ke ca khi nguoi dung go "__DELETED_ACCOUNT__") va bao
//     "Tài khoản này đã bị vô hiệu hóa!".
//   - Sai mat khau -> GIU NGUYEN ID da nhap, chi nhap lai mat khau.
//   - ESC / click nut "Quay lai" -> huy, quay ve menu chinh (tra ve "").
//   - De trong o Mat khau   -> quay lai buoc nhap ID (doi tai khoan).
// ------------------------------------------------------------
static string dangNhapNoiBo(string& idOut) {
    string ID, ThuMuc, vaiTro, thongBaoLoi;

    while (true) {
        clearScreen();
        veTieuDeForm("ĐĂNG NHẬP",
                     "(ESC hoặc nút Quay lại để trở về | Để trống mật khẩu để đổi ID)",
                     56);

        // ---------- Buoc 1: o ID ----------
        if (ID.empty()) {
            ID = nhapChuoiForm("ID", thongBaoLoi, true, 56, 16);
            thongBaoLoi = "";

            if (ID.empty()) {
                thongBaoLoi = "Vui lòng nhập ID!";
                continue;
            }

            string tienTo = (ID.size() >= 3) ? ID.substr(0, 3) : "";
            if (tienTo == "KH_")      { ThuMuc = "DataKhachHang"; vaiTro = "KH"; }
            else if (tienTo == "NV_") { ThuMuc = "DataNhanVien";  vaiTro = "NV"; }
            else if (tienTo == "QL_") { ThuMuc = "DataQuanLi";    vaiTro = "QL"; }
            else {
                thongBaoLoi = "ID không hợp lệ! (phải bắt đầu bằng QL_, NV_ hoặc KH_)";
                ID = "";
                continue;
            }

            if (!fs::exists(ThuMuc + "/" + ID + ".txt")) {
                thongBaoLoi = "Tài khoản không tồn tại!";
                ID = "";
                continue;
            }

            string mkKiemTra, hoTenKiemTra;
            if (docThongTinTaiKhoan(ThuMuc, ID, mkKiemTra, hoTenKiemTra)
                && mkKiemTra == MAT_KHAU_DA_XOA) {
                thongBaoLoi = "Tài khoản này đã bị vô hiệu hóa!";
                ID = "";
                continue;
            }
        } else {
            // Da co ID hop le tu lan truoc (vua sai mat khau) -> hien lai trong khung
            inTruongDaNhapForm("ID", ID, 56, 16);
        }

        // ---------- Buoc 2: o Mat khau ----------
        string MatKhau = nhapChuoiForm("Mật khẩu", thongBaoLoi, true, 56, 16);
        thongBaoLoi = "";

        if (MatKhau.empty()) { // De trong -> quay lai doi ID
            ID = "";
            continue;
        }

        string mkTrongFile, hoTenTrongFile;
        if (!docThongTinTaiKhoan(ThuMuc, ID, mkTrongFile, hoTenTrongFile)) {
            thongBaoLoi = "Không thể đọc dữ liệu tài khoản!";
            ID = "";
            continue;
        }

        // Tuyet doi khong cho dang nhap neu tai khoan trong file dang la
        // "__DELETED_ACCOUNT__" (ke ca khi nguoi dung co tinh go chuoi nay).
        if (mkTrongFile == MAT_KHAU_DA_XOA) {
            thongBaoLoi = "Tài khoản này đã bị vô hiệu hóa!";
            ID = "";
            continue;
        }

        if (mkTrongFile != MatKhau) {
            thongBaoLoi = "Sai mật khẩu! Vui lòng nhập lại.";
            continue; // Giu nguyen ID, ve lai form
        }

        idOut = ID;
        return vaiTro;
    }
}

// DangNhap(): ESC / click "Quay lai" o bat ky o nhap nao -> ve menu chinh
string DangNhap(string& idOut) {
    try {
        return dangNhapNoiBo(idOut);
    } catch (const HuyThaoTac&) {
        return "";
    }
}
