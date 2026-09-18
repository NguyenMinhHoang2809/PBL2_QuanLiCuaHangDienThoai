#include "KhachHang.h"
#include "Utils.h"      // nhapSoNguyen()
#include <iostream>
#include <fstream>
#include <filesystem>
#include <stack>

using namespace std;
namespace fs = std::filesystem;

KhachHang::KhachHang() : Nguoi() {}

// Constructor 2 tham so - goi lai constructor cua lop cha Nguoi(id, mk)
KhachHang::KhachHang(string id, string mk) : Nguoi(id, mk) {}

KhachHang::~KhachHang() {}

// ============================================================
// KhachHang CHI co quyen XEM LAI lich su mua hang cua chinh minh.
// Viec lap hoa don (mua/ban) la nghiep vu THUOC VE Nhan Vien - moi
// hoa don trong he thong deu do 1 NhanVienBanHang cu the tao ra
// (xem NhanVienBanHang::LapHoaDonBanHang), Khach hang khong tu
// checkout duoc.
// ============================================================
void KhachHang::hienThiMenu() {
    int luaChon;
    do {
        clearScreen(); // Dọn màn hình trước khi vẽ lại menu
        cout << "\n===== MENU KHACH HANG (" << getID() << ") =====\n";
        cout << "1. Xem lich su mua hang\n";
        cout << "0. Dang xuat\n";
        luaChon = nhapSoNguyen("Lua chon: ");

        switch (luaChon) {
            case 1: {
                clearScreen();
                // ----------------------------------------------------------
                // Xem lich su mua hang bang std::stack (LIFO):
                // Buoc 1: Duyet toan bo file trong DataHoaDon, file nao thuoc
                //         ve khach hang hien tai (dong "Ma Khach Hang: ID")
                //         thi push() duong dan file do vao stack.
                // Buoc 2: pop() lan luot ra khoi stack -> vi push theo thu tu
                //         duyet thu muc (thuong tang dan theo thoi gian tao),
                //         nen pop se cho ra hoa don duoc them SAU CUNG truoc
                //         -> dung nguyen tac LIFO (moi nhat hien len dau).
                // ----------------------------------------------------------
                string thuMucHoaDon = "DataHoaDon";
                stack<string> lichSuHoaDon;

                if (fs::exists(thuMucHoaDon) && fs::is_directory(thuMucHoaDon)) {
                    for (const auto& entry : fs::directory_iterator(thuMucHoaDon)) {
                        if (!entry.is_regular_file()) continue;

                        string duongDanFile = entry.path().string();
                        ifstream fileDoc(duongDanFile);
                        if (!fileDoc.is_open()) continue;

                        bool thuocVeKhachNay = false;
                        string dong;
                        while (getline(fileDoc, dong)) {
                            // Tim dong dang: "Ma Khach Hang: KH_xxx"
                            if (dong.rfind("Ma Khach Hang:", 0) == 0) {
                                string maKH = dong.substr(dong.find(":") + 1);
                                size_t viTriDau = maKH.find_first_not_of(" \t");
                                if (viTriDau != string::npos) maKH = maKH.substr(viTriDau);

                                if (maKH == getID()) {
                                    thuocVeKhachNay = true;
                                }
                                break; // dong nay chi xuat hien 1 lan trong file
                            }
                        }
                        fileDoc.close();

                        if (thuocVeKhachNay) {
                            lichSuHoaDon.push(duongDanFile); // PUSH vao Stack
                        }
                    }
                }

                if (lichSuHoaDon.empty()) {
                    cout << "Ban chua co hoa don mua hang nao.\n";
                    pauseScreen();
                    break;
                }

                cout << "\n===== LICH SU MUA HANG (moi nhat - LIFO) =====\n";
                while (!lichSuHoaDon.empty()) {
                    string duongDanFile = lichSuHoaDon.top();
                    lichSuHoaDon.pop(); // POP ra khoi Stack

                    ifstream fileDoc(duongDanFile);
                    if (!fileDoc.is_open()) continue;

                    // Neu hoa don la du lieu cu (tao truoc khi co truong
                    // "Ma Nhan Vien") thi se khong tim thay dong nay, gia
                    // tri mac dinh "(khong ro)" duoc dung de tranh hien
                    // thi rong hoac gay hieu lam.
                    string dong, maHD = "(khong ro)", maNV = "(khong ro)", tongTien = "(khong ro)";
                    while (getline(fileDoc, dong)) {
                        if (dong.rfind("Ma Hoa Don:", 0) == 0) {
                            maHD = dong.substr(dong.find(":") + 1);
                        } else if (dong.rfind("Ma Nhan Vien:", 0) == 0) {
                            maNV = dong.substr(dong.find(":") + 1);
                        } else if (dong.rfind("Tong Tien:", 0) == 0) {
                            tongTien = dong.substr(dong.find(":") + 1);
                        }
                    }
                    fileDoc.close();

                    cout << "- Ma HD:" << maHD << "  |  Nhan Vien Ban:" << maNV
                         << "  |  Tong Tien:" << tongTien << "\n";
                }
                cout << "================================================\n";
                pauseScreen();
                break;
            }
            case 0:
                cout << "Dang xuat...\n";
                break;
            default:
                cout << "Lua chon khong hop le!\n";
                pauseScreen();
        }
    } while (luaChon != 0);
}