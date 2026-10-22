#include "Account.h"
#include "TienIch.h"
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

// ------------------------------------------------------------
// TaoAccount: hien thi khung Form TAO TAI KHOAN phang can giua man hinh
// ------------------------------------------------------------
void TaoAccount(string vaiTroNguoiTao) {
    clearScreen();
    string ghiChu = (vaiTroNguoiTao == "NV")
        ? "(Nhân viên tạo tài khoản Khách hàng - bắt đầu bằng KH_)"
        : "(Định dạng ID: QL_..., NV_... hoặc KH_...)";
    veTieuDeForm("TẠO TÀI KHOẢN", ghiChu, 56);

    if (vaiTroNguoiTao == "KH") {
        inThongBaoForm("Khách hàng không có quyền tạo tài khoản!", LOAI_LOI);
        return;
    }

    string ID, MatKhau, ThuMuc;
    ID = nhapChuoiForm("Nhập ID", "", false, 56, 16);

    if (ID.size() >= 3 && ID.substr(0, 3) == "KH_") ThuMuc = "DataKhachHang";
    else if (ID.size() >= 3 && ID.substr(0, 3) == "NV_") ThuMuc = "DataNhanVien";
    else if (ID.size() >= 3 && ID.substr(0, 3) == "QL_") ThuMuc = "DataQuanLi";
    else {
        cout << "\n";
        inThongBaoForm("ID không hợp lệ! (phải bắt đầu bằng QL_, NV_ hoặc KH_)", LOAI_LOI);
        return;
    }

    // Rang buoc quyen tao tai khoan
    if (vaiTroNguoiTao == "NV" && ThuMuc != "DataKhachHang") {
        cout << "\n";
        inThongBaoForm("Nhân viên chỉ được tạo tài khoản Khách hàng!", LOAI_LOI);
        return;
    }

    fs::create_directories(ThuMuc);
    string TenFile = ThuMuc + "/" + ID + ".txt";

    if (fs::exists(TenFile)) {
        cout << "\n";
        inThongBaoForm("ID này đã tồn tại!", LOAI_LOI);
        return;
    }

    MatKhau = nhapChuoiForm("Mật khẩu", "", false, 56, 16);
    ofstream file(TenFile);
    file << ID << "\n" << MatKhau << "\n";
    file.close();

    cout << "\n";
    inThongBaoForm("Đã tạo tài khoản thành công!", LOAI_THANH_CONG);
}

// ------------------------------------------------------------
// DangNhap: giao dien FORM DANG NHAP PHANG (Flat UI) can giua man hinh:
//   - ID sai dinh dang / khong ton tai -> xoa ID, nhap lai ID.
//   - Sai mat khau -> GIU NGUYEN ID da nhap, chi nhap lai mat khau.
//   - Nhap "0" o o ID       -> huy, quay ve menu chinh (tra ve "").
//   - De trong o Mat khau   -> quay lai buoc nhap ID (doi tai khoan).
// ------------------------------------------------------------
string DangNhap(string& idOut) {
    string ID, ThuMuc, vaiTro, thongBaoLoi;

    while (true) {
        clearScreen();
        veTieuDeForm("ĐĂNG NHẬP",
                     "(Nhập 0 ở ô ID để quay lại | Để trống mật khẩu để đổi ID)",
                     56);

        // ---------- Buoc 1: o ID ----------
        if (ID.empty()) {
            ID = nhapChuoiForm("ID", thongBaoLoi, true, 56, 16);
            thongBaoLoi = "";

            if (ID == "0") return "";
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

        ifstream file(ThuMuc + "/" + ID + ".txt");
        string idTrongFile, mkTrongFile;
        getline(file, idTrongFile);
        getline(file, mkTrongFile);
        file.close();

        // Phong truong hop file bi sua tay tren Windows con sot '\r'
        if (!mkTrongFile.empty() && mkTrongFile.back() == '\r') mkTrongFile.pop_back();

        if (mkTrongFile != MatKhau) {
            thongBaoLoi = "Sai mật khẩu! Vui lòng nhập lại.";
            continue; // Giu nguyen ID, ve lai form
        }

        idOut = ID;
        return vaiTro;
    }
}
