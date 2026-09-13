#include "KhachHang.h"
#include "Utils.h"      // layNgayHeThong(), timMatHangTheoSoSeri()
#include "HoaDon.h"     // HoaDon, luuFile()
#include <iostream>
#include <fstream>
#include <filesystem>
#include <stack>
#include <set>
#include <iomanip>

using namespace std;
namespace fs = std::filesystem;

KhachHang::KhachHang() : Nguoi() {}

// Constructor 2 tham so - goi lai constructor cua lop cha Nguoi(id, mk)
KhachHang::KhachHang(string id, string mk) : Nguoi(id, mk) {}

KhachHang::~KhachHang() {}

// ------------------------------------------------------------
// Ham phu tro: tu dong sinh Ma Hoa Don duy nhat cho khach hang, dua
// tren ID khach hang + so thu tu file dang co trong DataHoaDon.
// Khach hang KHONG duoc tu nhap Ma Hoa Don (de tranh trung lap /
// nhap sai dinh dang) - khac voi Nhan Vien duoc phep tu dat Ma Hoa Don.
// ------------------------------------------------------------
static string sinhMaHoaDonTuDong(const string& idKhachHang) {
    string thuMucHoaDon = "DataHoaDon";
    int soLuongHienCo = 0;

    if (fs::exists(thuMucHoaDon) && fs::is_directory(thuMucHoaDon)) {
        for (const auto& entry : fs::directory_iterator(thuMucHoaDon)) {
            if (entry.is_regular_file()) soLuongHienCo++;
        }
    }

    return idKhachHang + "_" + to_string(soLuongHienCo + 1);
}

void KhachHang::hienThiMenu() {
    int luaChon;
    do {
        cout << "\n===== MENU KHACH HANG (" << getID() << ") =====\n";
        cout << "1. Mua hang\n";
        cout << "2. Xem lich su mua hang\n";
        cout << "0. Dang xuat\n";
        luaChon = nhapSoNguyen("Lua chon: ");

        switch (luaChon) {
            case 1: {
                // ----------------------------------------------------------
                // Mua hang: khach tu chon san pham theo So Seri, he thong tu
                // dong tinh tong tien va lap Hoa Don. maKhachHang BAT BUOC
                // lay tu getID() cua chinh doi tuong nay - khong cho nhap
                // tay, tranh truong hop mua ho / gia mao ID nguoi khac.
                // ----------------------------------------------------------
                string maHoaDon = sinhMaHoaDonTuDong(getID());
                string maKhachHang = getID(); // BAT BUOC lay tu chinh doi tuong

                double tongTien = 0;
                string soSeri;

                // Tap hop So Seri DA THEM vao don hang nay - moi may dien
                // thoai chi co 1 so seri duy nhat, khong the mua 2 lan
                // cung 1 may trong CUNG 1 don hang.
                set<string> daThemSoSeri;

                cout << "\n--- Nhap So Seri mat hang can mua (nhap 0 de ket thuc va chot don) ---\n";
                do {
                    cout << "So Seri: ";
                    cin >> soSeri;

                    if (soSeri == "0") break;

                    // Kiem tra trung lap TRUOC KHI tim trong DataMatHang -
                    // tranh cong don gia ban 2 lan cho cung 1 san pham.
                    if (daThemSoSeri.count(soSeri) > 0) {
                        cout << "So seri nay da duoc them vao don hang roi, khong the them lai!\n";
                        continue;
                    }

                    string tenMatHang;
                    double giaBan;

                    if (timMatHangTheoSoSeri(soSeri, tenMatHang, giaBan)) {
                        tongTien += giaBan;
                        daThemSoSeri.insert(soSeri); // Danh dau seri nay DA duoc them
                        cout << "Da them " << tenMatHang << " - Gia: "
                             << fixed << setprecision(0) << giaBan << "\n";
                    } else {
                        cout << "Khong ton tai so seri nay!\n";
                    }

                } while (soSeri != "0");

                if (tongTien <= 0) {
                    cout << "Ban chua chon mat hang nao, huy don mua hang!\n";
                    break;
                }

                HoaDon hoaDonMoi(maHoaDon, maKhachHang, layNgayHeThong(), tongTien);
                hoaDonMoi.luuFile();

                cout << "Tong tien don hang: " << fixed << setprecision(0) << tongTien << "\n";
                break;
            }
            case 2: {
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
                    break;
                }

                cout << "\n===== LICH SU MUA HANG (moi nhat - LIFO) =====\n";
                while (!lichSuHoaDon.empty()) {
                    string duongDanFile = lichSuHoaDon.top();
                    lichSuHoaDon.pop(); // POP ra khoi Stack

                    ifstream fileDoc(duongDanFile);
                    if (!fileDoc.is_open()) continue;

                    string dong, maHD = "(khong ro)", tongTien = "(khong ro)";
                    while (getline(fileDoc, dong)) {
                        if (dong.rfind("Ma Hoa Don:", 0) == 0) {
                            maHD = dong.substr(dong.find(":") + 1);
                        } else if (dong.rfind("Tong Tien:", 0) == 0) {
                            tongTien = dong.substr(dong.find(":") + 1);
                        }
                    }
                    fileDoc.close();

                    cout << "- Ma HD:" << maHD << "  |  Tong Tien:" << tongTien << "\n";
                }
                cout << "================================================\n";
                break;
            }
            case 0:
                cout << "Dang xuat...\n";
                break;
            default:
                cout << "Lua chon khong hop le!\n";
        }
    } while (luaChon != 0);
}