#include "ChuCuaHang.h"
#include "NhapHangHoa.h"   // TaoMatHang()
#include "Account.h"       // TaoAccount()
#include "Utils.h"         // nhapSoNguyen()
#include <iostream>

using namespace std;

ChuCuaHang::ChuCuaHang() : Nguoi() {}

// Constructor 2 tham so - goi lai constructor cua lop cha Nguoi(id, mk)
ChuCuaHang::ChuCuaHang(string id, string mk) : Nguoi(id, mk) {}

ChuCuaHang::~ChuCuaHang() {}

void ChuCuaHang::hienThiMenu() {
    int luaChon;
    do {
        cout << "\n===== MENU CHU CUA HANG (" << getID() << ") =====\n";
        cout << "1. Nhap hang hoa moi\n";
        cout << "2. Tao tai khoan Nhan vien\n";
        cout << "0. Dang xuat\n";
        luaChon = nhapSoNguyen("Lua chon: ");

        switch (luaChon) {
            case 1:
                TaoMatHang();
                break;
            case 2:
                TaoAccount("QL");
                break;
            case 0:
                cout << "Dang xuat...\n";
                break;
            default:
                cout << "Lua chon khong hop le!\n";
        }
    } while (luaChon != 0);
}