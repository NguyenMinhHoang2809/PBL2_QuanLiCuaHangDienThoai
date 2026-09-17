#include "KhoHang.h"
#include "Utils.h"      // nhapSoNguyen(), nhapSoThucKhongAm(), dinhDangTien()
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;

// ------------------------------------------------------------
// docToanBoKhoHang(): duyet DE QUY toan bo DataMatHang (vi moi dong
// may nam trong 1 thu muc con rieng), doc tung file .txt thanh mot
// ban ghi ThongTinMatHang. Doc HET vao bo nho 1 lan roi moi sap xep/
// loc, thay vi doc lai o dia moi lan can - nhanh va don gian hon.
// ------------------------------------------------------------
vector<ThongTinMatHang> docToanBoKhoHang() {
    vector<ThongTinMatHang> danhSach;
    string thuMucGoc = "DataMatHang";

    if (!fs::exists(thuMucGoc) || !fs::is_directory(thuMucGoc)) {
        return danhSach; // Kho rong -> tra ve vector rong
    }

    for (const auto& entry : fs::recursive_directory_iterator(thuMucGoc)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".txt") continue;

        ifstream fileDoc(entry.path());
        if (!fileDoc.is_open()) continue;

        ThongTinMatHang mh;
        mh.giaNhap = 0;
        mh.giaBan = 0;
        mh.daBan = false;
        mh.maKhachHangDaMua = "";

        string dong, giaNhapStr = "", giaBanStr = "";
        while (getline(fileDoc, dong)) {
            string giaTri;
            size_t viTriHai = dong.find(":");
            if (viTriHai != string::npos) {
                giaTri = dong.substr(viTriHai + 1);
                size_t vt = giaTri.find_first_not_of(" \t");
                giaTri = (vt == string::npos) ? "" : giaTri.substr(vt);
            }

            if (dong.rfind("Ten Mat Hang:", 0) == 0)            mh.tenMatHang = giaTri;
            else if (dong.rfind("So Seri:", 0) == 0)            mh.soSeri = giaTri;
            else if (dong.rfind("Gia Nhap:", 0) == 0)           giaNhapStr = giaTri;
            else if (dong.rfind("Gia Ban:", 0) == 0)            giaBanStr = giaTri;
            else if (dong.rfind("Tinh Trang:", 0) == 0)         mh.daBan = (giaTri == "Da Ban");
            else if (dong.rfind("Ma Khach Hang Da Mua:", 0) == 0) mh.maKhachHangDaMua = giaTri;
        }
        fileDoc.close();

        // File loi dinh dang gia -> bo qua ban ghi do, khong lam hong
        // toan bo bang danh sach.
        try {
            if (!giaNhapStr.empty()) mh.giaNhap = stod(giaNhapStr);
            if (!giaBanStr.empty())  mh.giaBan = stod(giaBanStr);
        } catch (...) {
            continue;
        }

        // Ten file (bo duoi .txt) chinh la So Seri - dung lam phuong an
        // du phong neu trong file thieu dong "So Seri:".
        if (mh.soSeri.empty()) mh.soSeri = entry.path().stem().string();
        if (mh.tenMatHang.empty()) mh.tenMatHang = "(khong ro)";

        danhSach.push_back(mh);
    }

    return danhSach;
}

// ------------------------------------------------------------
// XuatDanhSachKhoHang(): in bang danh sach kho, co menu chon kieu
// sap xep. Cot "Gia Nhap" CHI hien khi hienGiaNhap == true (Quan Ly) -
// Nhan Vien khong duoc thay gia nhap vi day la thong tin kinh doanh
// noi bo (biet gia nhap la biet bien loi nhuan cua cua hang).
// ------------------------------------------------------------
void XuatDanhSachKhoHang(bool hienGiaNhap) {
    vector<ThongTinMatHang> danhSach = docToanBoKhoHang();

    if (danhSach.empty()) {
        cout << "Kho hang hien dang rong!\n";
        return;
    }

    cout << "\nSap xep danh sach theo:\n";
    cout << "1. Gia ban TU CAO den THAP\n";
    cout << "2. Gia ban TU THAP den CAO\n";
    cout << "3. Ten mat hang (A-Z)\n";
    cout << "4. Chi xem hang CHUA BAN (gia tu thap den cao)\n";
    int kieuSapXep = nhapSoNguyen("Lua chon: ");

    if (kieuSapXep == 4) {
        // Loc bo cac may DA BAN ra khoi danh sach truoc khi sap xep
        vector<ThongTinMatHang> conHang;
        for (const auto& mh : danhSach) {
            if (!mh.daBan) conHang.push_back(mh);
        }
        danhSach = conHang;

        if (danhSach.empty()) {
            cout << "Khong con mat hang nao chua ban trong kho!\n";
            return;
        }
    }

    switch (kieuSapXep) {
        case 1:
            sort(danhSach.begin(), danhSach.end(),
                 [](const ThongTinMatHang& a, const ThongTinMatHang& b) {
                     return a.giaBan > b.giaBan;
                 });
            break;
        case 3:
            sort(danhSach.begin(), danhSach.end(),
                 [](const ThongTinMatHang& a, const ThongTinMatHang& b) {
                     return a.tenMatHang < b.tenMatHang;
                 });
            break;
        case 2:
        case 4:
        default:
            sort(danhSach.begin(), danhSach.end(),
                 [](const ThongTinMatHang& a, const ThongTinMatHang& b) {
                     return a.giaBan < b.giaBan;
                 });
            break;
    }

    cout << "\n================ DANH SACH KHO HANG ================\n";

    if (hienGiaNhap) {
        // ----- Ban danh cho QUAN LY: co ca Gia Nhap -----
        cout << "-------------------------------------------------------------------------------------------------\n";
        cout << "| " << left << setw(4) << "STT"
             << "| " << setw(22) << "Ten mat hang"
             << "| " << setw(14) << "So Seri"
             << "| " << setw(16) << "Gia nhap (VND)"
             << "| " << setw(16) << "Gia ban (VND)"
             << "| " << setw(10) << "Tinh trang" << " |\n";
        cout << "-------------------------------------------------------------------------------------------------\n";

        int stt = 0;
        for (const auto& mh : danhSach) {
            stt++;
            cout << "| " << left << setw(4) << stt
                 << "| " << setw(22) << mh.tenMatHang
                 << "| " << setw(14) << mh.soSeri
                 << "| " << setw(16) << dinhDangTien(mh.giaNhap)
                 << "| " << setw(16) << dinhDangTien(mh.giaBan)
                 << "| " << setw(10) << (mh.daBan ? "Da Ban" : "Chua Ban") << " |\n";
        }
        cout << "-------------------------------------------------------------------------------------------------\n";
    } else {
        // ----- Ban danh cho NHAN VIEN: KHONG co cot Gia Nhap -----
        cout << "--------------------------------------------------------------------------------\n";
        cout << "| " << left << setw(4) << "STT"
             << "| " << setw(24) << "Ten mat hang"
             << "| " << setw(16) << "So Seri"
             << "| " << setw(16) << "Gia ban (VND)"
             << "| " << setw(10) << "Tinh trang" << " |\n";
        cout << "--------------------------------------------------------------------------------\n";

        int stt = 0;
        for (const auto& mh : danhSach) {
            stt++;
            cout << "| " << left << setw(4) << stt
                 << "| " << setw(24) << mh.tenMatHang
                 << "| " << setw(16) << mh.soSeri
                 << "| " << setw(16) << dinhDangTien(mh.giaBan)
                 << "| " << setw(10) << (mh.daBan ? "Da Ban" : "Chua Ban") << " |\n";
        }
        cout << "--------------------------------------------------------------------------------\n";
    }

    cout << "Tong so luong: " << danhSach.size() << " thiet bi.\n";
}

// ------------------------------------------------------------
// TuVanBanHang(): nhap ngan sach cua khach, loc ra cac may CHUA BAN
// co Gia Ban <= ngan sach, sap xep tang dan roi goi y 3 muc:
//   - Toi uu chi phi : may re nhat (tiet kiem nhat)
//   - Trung binh     : may o giua danh sach (can bang gia/chat luong)
//   - Tot nhat       : may dat nhat van nam trong ngan sach
// Neu it lua chon hon 3 may thi chi hien cac muc co du lieu.
// ------------------------------------------------------------
void TuVanBanHang() {
    vector<ThongTinMatHang> danhSach = docToanBoKhoHang();

    if (danhSach.empty()) {
        cout << "Kho hang hien dang rong, khong the tu van!\n";
        return;
    }

    double nganSach = nhapSoThucKhongAm("Nhap so tien khach hang hien co (VND): ");

    // Chi tu van may CHUA BAN - may da ban khong the ban lai cho khach khac
    vector<ThongTinMatHang> phuHop;
    for (const auto& mh : danhSach) {
        if (!mh.daBan && mh.giaBan <= nganSach) phuHop.push_back(mh);
    }

    if (phuHop.empty()) {
        cout << "\nRat tiec, khong co san pham nao phu hop voi ngan sach "
             << dinhDangTien(nganSach) << " VND!\n";
        return;
    }

    sort(phuHop.begin(), phuHop.end(),
         [](const ThongTinMatHang& a, const ThongTinMatHang& b) {
             return a.giaBan < b.giaBan;
         });

    cout << "\n================ KET QUA TU VAN BAN HANG ================\n";
    cout << "Ngan sach: " << dinhDangTien(nganSach) << " VND\n";
    cout << "So may phu hop trong kho: " << phuHop.size() << "\n";
    cout << "-------------------------------------------------------------------------------------------------\n";
    cout << "| " << left << setw(18) << "Tieu chi"
         << "| " << setw(22) << "Ten mat hang"
         << "| " << setw(14) << "So Seri"
         << "| " << setw(16) << "Gia ban (VND)"
         << "| " << setw(16) << "Tiet kiem (VND)" << " |\n";
    cout << "-------------------------------------------------------------------------------------------------\n";

    // 1. Toi uu chi phi - may re nhat
    const ThongTinMatHang& reNhat = phuHop.front();
    cout << "| " << left << setw(18) << "1. Toi uu chi phi"
         << "| " << setw(22) << reNhat.tenMatHang
         << "| " << setw(14) << reNhat.soSeri
         << "| " << setw(16) << dinhDangTien(reNhat.giaBan)
         << "| " << setw(16) << dinhDangTien(nganSach - reNhat.giaBan) << " |\n";

    // 2. Trung binh - chi hien khi co tu 3 may tro len, neu khong muc
    // nay se trung lap voi muc "re nhat" hoac "tot nhat".
    if (phuHop.size() >= 3) {
        const ThongTinMatHang& trungBinh = phuHop[phuHop.size() / 2];
        cout << "| " << left << setw(18) << "2. Trung binh"
             << "| " << setw(22) << trungBinh.tenMatHang
             << "| " << setw(14) << trungBinh.soSeri
             << "| " << setw(16) << dinhDangTien(trungBinh.giaBan)
             << "| " << setw(16) << dinhDangTien(nganSach - trungBinh.giaBan) << " |\n";
    }

    // 3. Tot nhat - may dat nhat van trong ngan sach
    if (phuHop.size() >= 2) {
        const ThongTinMatHang& totNhat = phuHop.back();
        string tenMuc = (phuHop.size() == 2) ? "2. Tot nhat" : "3. Tot nhat";
        cout << "| " << left << setw(18) << tenMuc
             << "| " << setw(22) << totNhat.tenMatHang
             << "| " << setw(14) << totNhat.soSeri
             << "| " << setw(16) << dinhDangTien(totNhat.giaBan)
             << "| " << setw(16) << dinhDangTien(nganSach - totNhat.giaBan) << " |\n";
    }

    cout << "-------------------------------------------------------------------------------------------------\n";
}