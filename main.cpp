#include <iostream>
#include <filesystem>
#include "Account.h"
#include "NhapHangHoa.h"
#include "Nguoi.h"
#include "ChuCuaHang.h"
#include "NhanVienBanHang.h"
#include "KhachHang.h"
#include "TienIch.h"

using namespace std;
namespace fs = std::filesystem;

// Neu he thong chua co tai khoan Quan Ly nao, bat buoc phai tao truoc
void khoiTaoTaiKhoanQuanLyDauTien() {
    if (!fs::exists("DataQuanLi") || fs::is_empty("DataQuanLi")) {
        TaoAccount("QL");
        pauseScreen();
    }
}

// ============================================================
// FACTORY: dua vao vaiTro (tra ve tu DangNhap) de khoi tao dung
// lop dan xuat tuong ung, tra ve thong qua con tro lop co so Nguoi*.
// ============================================================
Nguoi* taoNguoiDungTheoVaiTro(const string& vaiTro, const string& id) {
    if (vaiTro == "QL") return new ChuCuaHang(id, "");
    if (vaiTro == "NV") return new NhanVienBanHang(id, "");
    if (vaiTro == "KH") return new KhachHang(id, "");
    return nullptr;
}

int main() {
    khoiTaoConsole();

    khoiTaoTaiKhoanQuanLyDauTien();

    int luaChon;
    do {
        const vector<string> cacMuc = { "Đăng nhập", "Thoát chương trình" };
        int chiSo = chonMenu("HOÀNG CỬU BẢO MOBILE", cacMuc);
        luaChon = (chiSo == static_cast<int>(cacMuc.size()) - 1) ? 0 : chiSo + 1;

        if (luaChon == 1) {
            string id;
            string vaiTro = DangNhap(id);

            if (!vaiTro.empty()) {
                Nguoi* nguoiDungHienTai = taoNguoiDungTheoVaiTro(vaiTro, id);

                if (nguoiDungHienTai != nullptr) {
                    nguoiDungHienTai->hienThiMenu();
                    delete nguoiDungHienTai;
                    nguoiDungHienTai = nullptr;
                }
            }
        } else if (luaChon != 0) {
            inThongBaoForm("Lựa chọn không hợp lệ!", LOAI_LOI);
            pauseScreen();
        }

    } while (luaChon != 0);

    clearScreen();
    veTieuDeForm("HOÀNG CỬU BẢO MOBILE", "Cảm ơn quý khách đã sử dụng chương trình!", 56);
    return 0;
}

