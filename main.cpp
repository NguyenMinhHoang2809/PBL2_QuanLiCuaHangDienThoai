#include <iostream>
#include <filesystem>
#include "Account.h"
#include "NhapHangHoa.h"
#include "Nguoi.h"
#include "ChuCuaHang.h"
#include "NhanVienBanHang.h"
#include "KhachHang.h"
#include "Utils.h"

using namespace std;
namespace fs = std::filesystem;

// Neu he thong chua co tai khoan Quan Ly nao, bat buoc phai tao truoc
// (vi NV chi tao duoc KH, KH khong tao duoc ai ca -> phai co QL goc)
void khoiTaoTaiKhoanQuanLyDauTien() {
    if (!fs::exists("DataQuanLi") || fs::is_empty("DataQuanLi")) {
        cout << "He thong chua co tai khoan Quan Ly nao.\n";
        cout << "Vui long tao tai khoan Quan Ly dau tien (ID phai bat dau bang QL_).\n";
        TaoAccount("QL");
    }
}

// ============================================================
// FACTORY: dua vao vaiTro (tra ve tu DangNhap) de khoi tao dung
// lop dan xuat tuong ung, tra ve thong qua con tro lop co so Nguoi*.
// Mat khau khong can luu lai trong RAM sau khi da xac thuc xong.
// ============================================================
Nguoi* taoNguoiDungTheoVaiTro(const string& vaiTro, const string& id) {
    if (vaiTro == "QL") return new ChuCuaHang(id, "");
    if (vaiTro == "NV") return new NhanVienBanHang(id, "");
    if (vaiTro == "KH") return new KhachHang(id, "");
    return nullptr;
}

int main() {
    khoiTaoTaiKhoanQuanLyDauTien();

    int luaChon;
    do {
        clearScreen(); // Dọn màn hình trước khi vẽ lại menu - tránh lịch sử chồng chất
        cout << "\n========================================\n";  
        cout << "   HOANG CUU BAO MOBILE \n";
        cout << "========================================\n";
        cout << "1. Dang nhap\n";
        cout << "0. Thoat chuong trinh\n";
        luaChon = nhapSoNguyen("Lua chon: ");

        if (luaChon == 1) {
            clearScreen(); // Dọn màn hình ngay khi chọn Đăng nhập, chỉ còn ID/Mật khẩu
            cout << "===== DANG NHAP =====\n\n";

            string id;
            string vaiTro = DangNhap(id);

            if (!vaiTro.empty()) {
                // Tao doi tuong dung theo vai tro (Factory)
                Nguoi* nguoiDungHienTai = taoNguoiDungTheoVaiTro(vaiTro, id);

                if (nguoiDungHienTai != nullptr) {
                    nguoiDungHienTai->hienThiMenu();   // Goi ham DA HINH (polymorphism)
                    delete nguoiDungHienTai;            // Giai phong bo nho - chong memory leak
                    nguoiDungHienTai = nullptr;
                }
            } else {
                // Dang nhap that bai - DangNhap() da tu in loi (vd: "Sai mat
                // khau!"). Dung lai de nguoi dung KIP DOC thong bao loi nay
                // truoc khi vong lap quay lai clearScreen() xoa sach no.
                pauseScreen();
            }
        } else if (luaChon != 0) {
            cout << "Lua chon khong hop le!\n";
            pauseScreen();
        }

    } while (luaChon != 0);

    cout << "Cam on da su dung chuong trinh!\n";
    return 0;
}