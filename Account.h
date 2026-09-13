#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

// ============================================================
// Module quan ly Tai khoan (dung chung cho ca he thong phan quyen)
// Duoc goi lai boi: ChuCuaHang::TaoAccount("QL")
//                    NhanVienBanHang::TaoAccount("NV")
// ============================================================

// Tao tai khoan moi.
// vaiTroNguoiTao: vai tro cua NGUOI DANG THUC HIEN thao tac tao ("QL" | "NV" | "KH")
//   - QL : duoc tao bat ky loai tai khoan nao (QL_, NV_, KH_)
//   - NV : chi duoc tao tai khoan Khach hang (KH_)
//   - KH : khong co quyen tao tai khoan
void TaoAccount(std::string vaiTroNguoiTao);

// Dang nhap vao he thong.
// idOut: tham chieu dau ra, se duoc gan ID neu dang nhap thanh cong.
// Tra ve: "QL" / "NV" / "KH" neu thanh cong, chuoi rong "" neu that bai.
std::string DangNhap(std::string& idOut);

#endif // ACCOUNT_H