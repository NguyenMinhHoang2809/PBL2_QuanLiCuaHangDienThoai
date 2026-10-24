#include "KhoHang.h"
#include "TienIch.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;

// Can le trai chuoi UTF-8 theo so ky tu HIEN THI thuc te (doRongHienThi)
// de bang khong bi lech cot khi co tieng Viet co dau.
static string canTraiUTF8(const string& s, int rongCot) {
    int hienThi = doRongHienThi(s);
    if (hienThi >= rongCot) return s;
    return s + string(rongCot - hienThi, ' ');
}

// ------------------------------------------------------------
// docToanBoKhoHang(): duyet DE QUY toan bo DataMatHang, doc tung file
// .txt thanh mot ban ghi ThongTinMatHang.
// ------------------------------------------------------------
vector<ThongTinMatHang> docToanBoKhoHang() {
    vector<ThongTinMatHang> danhSach;
    string thuMucGoc = "DataMatHang";

    if (!fs::exists(thuMucGoc) || !fs::is_directory(thuMucGoc)) {
        return danhSach;
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

            if (dong.rfind("Ten Mat Hang:", 0) == 0)              mh.tenMatHang = giaTri;
            else if (dong.rfind("So Seri:", 0) == 0)              mh.soSeri = giaTri;
            else if (dong.rfind("Gia Nhap:", 0) == 0)             giaNhapStr = giaTri;
            else if (dong.rfind("Gia Ban:", 0) == 0)              giaBanStr = giaTri;
            else if (dong.rfind("Tinh Trang:", 0) == 0)           mh.daBan = (giaTri == "Da Ban");
            else if (dong.rfind("Ma Khach Hang Da Mua:", 0) == 0) mh.maKhachHangDaMua = giaTri;
        }
        fileDoc.close();

        try {
            if (!giaNhapStr.empty()) mh.giaNhap = stod(giaNhapStr);
            if (!giaBanStr.empty())  mh.giaBan = stod(giaBanStr);
        } catch (...) {
            continue;
        }

        if (mh.soSeri.empty()) mh.soSeri = entry.path().stem().string();
        if (mh.tenMatHang.empty()) mh.tenMatHang = "(không rõ)";

        danhSach.push_back(mh);
    }

    return danhSach;
}

// ------------------------------------------------------------
// XuatDanhSachKhoHang(): in bang danh sach kho can giua man hinh, co
// menu chon kieu sap xep.
// ------------------------------------------------------------
void XuatDanhSachKhoHang(bool hienGiaNhap) {
    vector<ThongTinMatHang> danhSach = docToanBoKhoHang();

    if (danhSach.empty()) {
        clearScreen();
        veTieuDeForm("DANH SÁCH KHO HÀNG");
        inThongBaoForm("Kho hàng hiện đang rỗng!", LOAI_GHI_CHU);
        return;
    }

    const vector<string> cacMuc = {
        "Giá bán TỪ CAO đến THẤP",
        "Giá bán TỪ THẤP đến CAO",
        "Tên mặt hàng (A-Z)",
        "Chỉ xem hàng CHƯA BÁN (giá từ thấp đến cao)",
        "Quay lại"
    };
    int chiSo = chonMenu("SẮP XẾP DANH SÁCH KHO HÀNG THEO", cacMuc);
    if (chiSo == static_cast<int>(cacMuc.size()) - 1) throw HuyThaoTac(); // Quay lai menu
    int kieuSapXep = chiSo + 1;

    if (kieuSapXep == 4) {
        vector<ThongTinMatHang> conHang;
        for (const auto& mh : danhSach) {
            if (!mh.daBan) conHang.push_back(mh);
        }
        danhSach = conHang;

        if (danhSach.empty()) {
            clearScreen();
            veTieuDeForm("DANH SÁCH KHO HÀNG");
            inThongBaoForm("Không còn mặt hàng nào chưa bán trong kho!", LOAI_GHI_CHU);
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

    clearScreen();
    int rongBang = hienGiaNhap ? 94 : 76;
    veTieuDeForm("DANH SÁCH KHO HÀNG",
                 "Tổng số lượng: " + to_string(danhSach.size()) + " thiết bị",
                 rongBang);

    int leTrai = tinhLeTraiCanGiua(rongBang);
    string khoangLe(leTrai, ' ');

    if (hienGiaNhap) {
        string tieuDeCot = "  " + canTraiUTF8("STT", 5)
                         + canTraiUTF8("Tên mặt hàng", 24)
                         + canTraiUTF8("Số Seri", 15)
                         + canTraiUTF8("Giá nhập (VNĐ)", 18)
                         + canTraiUTF8("Giá bán (VNĐ)", 18)
                         + canTraiUTF8("Tình trạng", 12);
        cout << khoangLe << "\033[48;5;236m\033[93;1m" << canTraiUTF8(tieuDeCot, rongBang) << "\033[0m\n";

        int stt = 0;
        for (const auto& mh : danhSach) {
            stt++;
            const char* mauNen = (stt % 2 == 1) ? "\033[48;5;235m\033[97m" : "\033[48;5;237m\033[97m";
            string dong = "  " + canTraiUTF8(to_string(stt), 5)
                        + canTraiUTF8(mh.tenMatHang, 24)
                        + canTraiUTF8(mh.soSeri, 15)
                        + canTraiUTF8(dinhDangTien(mh.giaNhap), 18)
                        + canTraiUTF8(dinhDangTien(mh.giaBan), 18)
                        + canTraiUTF8(mh.daBan ? "Đã bán" : "Chưa bán", 12);
            cout << khoangLe << mauNen << canTraiUTF8(dong, rongBang) << "\033[0m\n";
        }
    } else {
        string tieuDeCot = "  " + canTraiUTF8("STT", 5)
                         + canTraiUTF8("Tên mặt hàng", 24)
                         + canTraiUTF8("Số Seri", 15)
                         + canTraiUTF8("Giá bán (VNĐ)", 18)
                         + canTraiUTF8("Tình trạng", 12);
        cout << khoangLe << "\033[48;5;236m\033[93;1m" << canTraiUTF8(tieuDeCot, rongBang) << "\033[0m\n";

        int stt = 0;
        for (const auto& mh : danhSach) {
            stt++;
            const char* mauNen = (stt % 2 == 1) ? "\033[48;5;235m\033[97m" : "\033[48;5;237m\033[97m";
            string dong = "  " + canTraiUTF8(to_string(stt), 5)
                        + canTraiUTF8(mh.tenMatHang, 24)
                        + canTraiUTF8(mh.soSeri, 15)
                        + canTraiUTF8(dinhDangTien(mh.giaBan), 18)
                        + canTraiUTF8(mh.daBan ? "Đã bán" : "Chưa bán", 12);
            cout << khoangLe << mauNen << canTraiUTF8(dong, rongBang) << "\033[0m\n";
        }
    }
}

// ------------------------------------------------------------
// TuVanBanHang(): nhap ngan sach cua khach trong khung Form phang can
// giua, loc ra cac may CHUA BAN co Gia Ban <= ngan sach va goi y 3 muc.
// ------------------------------------------------------------
void TuVanBanHang() {
    clearScreen();
    vector<ThongTinMatHang> danhSach = docToanBoKhoHang();

    veTieuDeForm("TƯ VẤN BÁN HÀNG",
                 "(Gợi ý thiết bị chưa bán phù hợp với ngân sách khách hàng)",
                 58);

    if (danhSach.empty()) {
        inThongBaoForm("Kho hàng hiện đang rỗng, không thể tư vấn!", LOAI_LOI);
        return;
    }

    double nganSach = nhapSoThucKhongAm("Ngân sách khách có (VNĐ)");

    vector<ThongTinMatHang> phuHop;
    for (const auto& mh : danhSach) {
        if (!mh.daBan && mh.giaBan <= nganSach) phuHop.push_back(mh);
    }

    if (phuHop.empty()) {
        cout << "\n";
        inThongBaoForm("Rất tiếc, không có sản phẩm nào phù hợp với ngân sách "
                       + dinhDangTien(nganSach) + " VNĐ!", LOAI_LOI);
        return;
    }

    sort(phuHop.begin(), phuHop.end(),
         [](const ThongTinMatHang& a, const ThongTinMatHang& b) {
             return a.giaBan < b.giaBan;
         });

    cout << "\n";
    int rongBang = 94;
    int leTrai = tinhLeTraiCanGiua(rongBang);
    string khoangLe(leTrai, ' ');

    inDongCanGiua("Ngân sách: " + dinhDangTien(nganSach) + " VNĐ  |  Số máy phù hợp: "
                  + to_string(phuHop.size()), "\033[92;1m");
    cout << "\n";

    string tieuDeCot = "  " + canTraiUTF8("Tiêu chí", 20)
                     + canTraiUTF8("Tên mặt hàng", 24)
                     + canTraiUTF8("Số Seri", 14)
                     + canTraiUTF8("Giá bán (VNĐ)", 17)
                     + canTraiUTF8("Tiết kiệm (VNĐ)", 17);
    cout << khoangLe << "\033[48;5;236m\033[93;1m" << canTraiUTF8(tieuDeCot, rongBang) << "\033[0m\n";

    auto inDongGoiY = [&](const string& tieuChi, const ThongTinMatHang& mh, bool dongLe) {
        const char* mauNen = dongLe ? "\033[48;5;235m\033[97m" : "\033[48;5;237m\033[97m";
        string dong = "  " + canTraiUTF8(tieuChi, 20)
                    + canTraiUTF8(mh.tenMatHang, 24)
                    + canTraiUTF8(mh.soSeri, 14)
                    + canTraiUTF8(dinhDangTien(mh.giaBan), 17)
                    + canTraiUTF8(dinhDangTien(nganSach - mh.giaBan), 17);
        cout << khoangLe << mauNen << canTraiUTF8(dong, rongBang) << "\033[0m\n";
    };

    // 1. Toi uu chi phi - may re nhat
    inDongGoiY("1. Tối ưu chi phí", phuHop.front(), true);

    // 2. Trung binh - chi hien khi co tu 3 may tro len
    if (phuHop.size() >= 3) {
        inDongGoiY("2. Trung bình", phuHop[phuHop.size() / 2], false);
    }

    // 3. Tot nhat - may dat nhat van trong ngan sach
    if (phuHop.size() >= 2) {
        string tenMuc = (phuHop.size() == 2) ? "2. Tốt nhất" : "3. Tốt nhất";
        inDongGoiY(tenMuc, phuHop.back(), phuHop.size() == 2 ? false : true);
    }
}
