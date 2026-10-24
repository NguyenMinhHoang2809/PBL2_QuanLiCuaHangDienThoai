#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

// ============================================================
// Module quan ly Tai khoan (dung chung cho ca he thong phan quyen)
// Duoc goi lai boi: ChuCuaHang -> TaoAccount("QL")
//                   NhanVienBanHang -> TaoAccount("NV")
// ============================================================

// Hang so danh dau tai khoan da bi xoa (vo hieu hoa). Khong nguoi dung
// nao duoc phep dat mat khau trung voi chuoi nay va tai khoan mang ma
// nay se khong the dang nhap cho den khi duoc tao lai.
const char* const MAT_KHAU_DA_XOA = "__DELETED_ACCOUNT__";

// Tao tai khoan moi (gom ID, Ho va ten, Mat khau). Neu ID da ton tai
// nhung dang o trang thai bi xoa (__DELETED_ACCOUNT__), cho phep tao
// lai (ghi de mat khau moi va ho ten moi) ma van giu nguyen lich su
// hoa don cu cua ID do.
void TaoAccount(std::string vaiTroNguoiTao);

// Doc thong tin tai khoan tu file <thuMuc>/<id>.txt (tuong thich nguoc
// voi cac file tai khoan cu chi co 2 dong ID + MatKhau, chua co HoTen).
bool docThongTinTaiKhoan(const std::string& thuMuc, const std::string& id,
                         std::string& matKhauRa, std::string& hoTenRa);

// Lay Ho va ten cua tai khoan (tra ve "(Chưa cập nhật)" neu tai khoan cu chua co ten).
std::string layHoTenTaiKhoan(const std::string& thuMuc, const std::string& id);

// Dang nhap vao he thong.
// idOut: tham chieu dau ra, se duoc gan ID neu dang nhap thanh cong.
// Tra ve: "QL" / "NV" / "KH" neu thanh cong, chuoi rong "" neu that bai.
std::string DangNhap(std::string& idOut);

#endif // ACCOUNT_H
