#include "KhachHang.h"
#include "TienIch.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <stack>

using namespace std;
namespace fs = std::filesystem;

KhachHang::KhachHang() : Nguoi() {}

KhachHang::KhachHang(string id, string mk, string hoTen) : Nguoi(id, mk, hoTen) {}

KhachHang::~KhachHang() {}

static string canTraiUTF8(const string& s, int rongCot) {
    int hienThi = doRongHienThi(s);
    if (hienThi >= rongCot) return s;
    return s + string(rongCot - hienThi, ' ');
}

static string catKhoangTrangDau(const string& s) {
    size_t vt = s.find_first_not_of(" \t");
    return (vt == string::npos) ? "" : s.substr(vt);
}

void KhachHang::hienThiMenu() {
    int luaChon;
    do {
        const vector<string> cacMuc = { "Xem lịch sử mua hàng", "Đăng xuất" };
        int chiSo = chonMenu("MENU KHÁCH HÀNG (" + getID() + ")", cacMuc);
        luaChon = (chiSo == static_cast<int>(cacMuc.size()) - 1) ? 0 : chiSo + 1;

        try {
            switch (luaChon) {
                case 1: {
                    clearScreen();
                    int rongBang = 72;
                    string thongTinPhu = "Khách hàng: " + getID();
                    if (!getHoTen().empty() && getHoTen() != "(Chưa cập nhật)") {
                        thongTinPhu += " - " + getHoTen();
                    }
                    thongTinPhu += " (Mới nhất trước - LIFO)";
                    veTieuDeForm("LỊCH SỬ MUA HÀNG", thongTinPhu, rongBang);

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
                                if (dong.rfind("Ma Khach Hang:", 0) == 0) {
                                    string maKH = catKhoangTrangDau(dong.substr(dong.find(":") + 1));
                                    if (maKH == getID()) {
                                        thuocVeKhachNay = true;
                                    }
                                    break;
                                }
                            }
                            fileDoc.close();

                            if (thuocVeKhachNay) {
                                lichSuHoaDon.push(duongDanFile);
                            }
                        }
                    }

                    if (lichSuHoaDon.empty()) {
                        inThongBaoForm("Bạn chưa có hóa đơn mua hàng nào.", LOAI_GHI_CHU);
                        pauseScreen();
                        break;
                    }

                    int leTrai = tinhLeTraiCanGiua(rongBang);
                    string khoangLe(leTrai, ' ');

                    string tieuDeCot = "  " + canTraiUTF8("Mã Hóa đơn", 16)
                                     + canTraiUTF8("Nhân viên bán", 18)
                                     + canTraiUTF8("Ngày lập", 16)
                                     + canTraiUTF8("Tổng tiền (VNĐ)", 20);
                    cout << khoangLe << "\033[48;5;236m\033[93;1m" << canTraiUTF8(tieuDeCot, rongBang) << "\033[0m\n";

                    int stt = 0;
                    while (!lichSuHoaDon.empty()) {
                        string duongDanFile = lichSuHoaDon.top();
                        lichSuHoaDon.pop();

                        ifstream fileDoc(duongDanFile);
                        if (!fileDoc.is_open()) continue;

                        string dong, maHD = "(không rõ)", maNV = "(không rõ)", ngayLap = "(không rõ)", tongTienStr = "0";
                        while (getline(fileDoc, dong)) {
                            if (dong.rfind("Ma Hoa Don:", 0) == 0) {
                                maHD = catKhoangTrangDau(dong.substr(dong.find(":") + 1));
                            } else if (dong.rfind("Ma Nhan Vien:", 0) == 0) {
                                maNV = catKhoangTrangDau(dong.substr(dong.find(":") + 1));
                            } else if (dong.rfind("Ngay Lap:", 0) == 0) {
                                ngayLap = catKhoangTrangDau(dong.substr(dong.find(":") + 1));
                            } else if (dong.rfind("Tong Tien:", 0) == 0) {
                                tongTienStr = catKhoangTrangDau(dong.substr(dong.find(":") + 1));
                            }
                        }
                        fileDoc.close();

                        string hienThiTien = tongTienStr;
                        try {
                            hienThiTien = dinhDangTien(stod(tongTienStr));
                        } catch (...) {}

                        stt++;
                        const char* mauNen = (stt % 2 == 1) ? "\033[48;5;235m\033[97m" : "\033[48;5;237m\033[97m";
                        string dongIn = "  " + canTraiUTF8(maHD, 16)
                                      + canTraiUTF8(maNV, 18)
                                      + canTraiUTF8(ngayLap, 16)
                                      + canTraiUTF8(hienThiTien, 20);
                        cout << khoangLe << mauNen << canTraiUTF8(dongIn, rongBang) << "\033[0m\n";
                    }

                    pauseScreen();
                    break;
                }
                case 0:
                    break;
                default:
                    break;
            }
        } catch (const HuyThaoTac&) {
            // Huy thao tac -> quay lai menu
        }
    } while (luaChon != 0);
}
