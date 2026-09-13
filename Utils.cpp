#include "Utils.h"
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <limits>

using namespace std;
namespace fs = std::filesystem;

// ------------------------------------------------------------
// layNgayHeThong(): lay ngay hien tai theo dinh dang dd-mm-yyyy.
// Dung ostringstream + setw/setfill (thuan C++) thay vi snprintf
// de tranh canh bao -Wformat-truncation cua GCC.
// ------------------------------------------------------------
string layNgayHeThong() {
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);

    ostringstream oss;
    oss << setw(2) << setfill('0') << ltm->tm_mday << "-"
        << setw(2) << setfill('0') << (1 + ltm->tm_mon) << "-"
        << setw(4) << setfill('0') << (1900 + ltm->tm_year);
    return oss.str();
}

// ------------------------------------------------------------
// timMatHangTheoSoSeri(): tim file <soSeri>.txt trong toan bo thu muc
// con cua DataMatHang (dung recursive_directory_iterator vi cau truc
// luu la DataMatHang/<TenMatHang>/<SoSeri>.txt - khong biet truoc
// TenMatHang). Neu tim thay, doc "Ten Mat Hang" va "Gia Ban" tra ve
// qua tham chieu (out parameter) va tra ve true.
// ------------------------------------------------------------
bool timMatHangTheoSoSeri(const string& soSeri, string& tenMatHangRa, double& giaBanRa) {
    string thuMucGoc = "DataMatHang";

    if (!fs::exists(thuMucGoc) || !fs::is_directory(thuMucGoc)) {
        return false;
    }

    for (const auto& entry : fs::recursive_directory_iterator(thuMucGoc)) {
        if (!entry.is_regular_file()) continue;

        // Ten file (khong bao gom duoi .txt) chinh la So Seri
        if (entry.path().stem().string() != soSeri) continue;

        ifstream fileDoc(entry.path());
        if (!fileDoc.is_open()) return false;

        string dong, tenMatHang = "", giaBanStr = "";
        while (getline(fileDoc, dong)) {
            if (dong.rfind("Ten Mat Hang:", 0) == 0) {
                tenMatHang = dong.substr(dong.find(":") + 1);
                size_t viTriDau = tenMatHang.find_first_not_of(" \t");
                if (viTriDau != string::npos) tenMatHang = tenMatHang.substr(viTriDau);
            } else if (dong.rfind("Gia Ban:", 0) == 0) {
                giaBanStr = dong.substr(dong.find(":") + 1);
                size_t viTriDau = giaBanStr.find_first_not_of(" \t");
                if (viTriDau != string::npos) giaBanStr = giaBanStr.substr(viTriDau);
            }
        }
        fileDoc.close();

        if (giaBanStr.empty()) return false;

        try {
            giaBanRa = stod(giaBanStr);
        } catch (...) {
            return false; // Gia Ban trong file bi loi dinh dang
        }

        tenMatHangRa = tenMatHang;
        return true;
    }

    return false; // Da duyet het DataMatHang ma khong tim thay So Seri
}

// ------------------------------------------------------------
// nhapSoNguyen(): boc "cin >> giaTri" (kieu int) trong vong lap.
// Neu nguoi dung go chu, cin chuyen sang failbit va KHONG gan gia
// tri cho giaTri -> bat buoc phai cin.clear() (xoa co loi) roi
// cin.ignore(...) (don sach ky tu rac con ket trong buffer), neu
// khong lan cin >> ke tiep se doc lai dung ky tu rac do va lap loi
// vo han (treo cung Console).
// ------------------------------------------------------------
int nhapSoNguyen(const string& thongBao) {
    int giaTri;
    while (true) {
        cout << thongBao;
        cin >> giaTri;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Du lieu khong hop le! Vui long nhap lai so nguyen.\n";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return giaTri;
    }
}

// ------------------------------------------------------------
// nhapSoThucKhongAm(): tuong tu nhapSoNguyen() nhung cho kieu double
// va bo sung kiem tra khong cho phep gia tri am (dung cho Gia Nhap /
// Gia Ban - khong the co gia san pham am).
// ------------------------------------------------------------
double nhapSoThucKhongAm(const string& thongBao) {
    double giaTri;
    while (true) {
        cout << thongBao;
        cin >> giaTri;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Du lieu khong hop le! Vui long nhap lai so.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (giaTri < 0) {
            cout << "Gia tri khong duoc am! Vui long nhap lai.\n";
            continue;
        }

        return giaTri;
    }
}