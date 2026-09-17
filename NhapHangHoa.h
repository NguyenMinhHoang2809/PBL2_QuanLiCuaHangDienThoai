#ifndef NHAPHANGHOA_H
#define NHAPHANGHOA_H

// ============================================================
// Chuc nang NHAP HANG HOA MOI - CHI danh cho ChuCuaHang (Quan Ly)
// Duoc goi tu: ChuCuaHang::hienThiMenu() -> TaoMatHang()
// ============================================================
void TaoMatHang();

// ============================================================
// NHAP HANG LOAT tu mot file .txt - CHI danh cho ChuCuaHang (Quan Ly)
// Moi dong trong file la 1 thiet bi, dinh dang:
//     <Ten Mat Hang> <So Seri> <Gia Nhap> <Gia Ban>
// Vi du: iphone15 a1508 15000000 18000000
// Cung chap nhan dinh dang phan cach bang dau phay:
//     iphone 15 Pro,a1508,15000000,18000000
// ============================================================
void NhapHangLoatTuFile();

#endif // NHAPHANGHOA_H