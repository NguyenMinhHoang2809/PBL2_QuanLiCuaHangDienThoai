#include "NhapHangHoa.h"
#include "Utils.h"      // nhapSoThucKhongAm(), chuanHoaTenThuMuc(), soSeriDaTonTai()
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

// Luu y: chuanHoaTenThuMuc() da duoc chuyen sang Utils.cpp de ca
// TaoMatHang() (nhap 1 san pham) lan NhapHangLoatTuFile() (nhap hang
// loat) deu dung CHUNG mot logic chuan hoa - neu 2 noi chuan hoa khac
// nhau, cung 1 san pham se bi tach thanh 2 thu muc rieng.

// ------------------------------------------------------------
// TaoMatHang: giu nguyen logic tham chieu, nhung GiaNhap/GiaBan
// duoc doc bang nhapSoThucKhongAm() (kieu double, khong cho gia am)
// thay vi cin >> string nhu ban dau - tranh luu gia tri "rac"
// (chu, ky hieu...) vao file du lieu san pham.
// ------------------------------------------------------------
void TaoMatHang() {
    string TenMatHang, SoSeri;

    // Dung getline(cin >> ws, ...) thay vi cin >> de nhan duoc ten mat hang
    // co khoang trang (vd: "iPhone 15 Pro", "Samsung Galaxy Tab S9").
    // "cin >> ws" se bo qua het khoang trang/newline con sot lai trong buffer
    // truoc khi getline bat dau doc, tranh loi "troi lenh".
    cout << "Nhap Ten Mat Hang: "; getline(cin >> ws, TenMatHang);
    cout << "So seri: "; cin >> SoSeri;

    string ThuMucGoc = "DataMatHang";
    // Dung ten thu muc DA CHUAN HOA (khong khoang trang) - tranh tao
    // trung thu muc cho cung 1 san pham khi go khoang trang khac nhau.
    string ThuMucCon = ThuMucGoc + "/" + chuanHoaTenThuMuc(TenMatHang);
    string TenFile = ThuMucCon + "/" + SoSeri + ".txt";

    // QUAN TRONG: kiem tra So Seri TRUNG LAP tren TOAN BO DataMatHang
    // (moi dong may, moi thu muc con), KHONG CHI trong thu muc cua rieng
    // mat hang nay. Neu chi dung fs::exists(TenFile) nhu truoc day, seri
    // se chi bi chan trung khi CUNG mot ten mat hang - hai dong may khac
    // nhau (vd: "iphone 14" va "iphone 14 pro") van co the bi gan trung
    // seri vi duong dan file khac nhau, du ve nghiep vu 1 seri chi thuoc
    // ve DUY NHAT 1 chiec may vat ly tren toan he thong.
    if (soSeriDaTonTai(SoSeri)) {
        cout << "So Seri nay da ton tai (o mot mat hang khac)! So Seri phai la duy nhat tren toan he thong." << endl;
        return;
    }

    double GiaNhap = nhapSoThucKhongAm("Gia Nhap: ");
    double GiaBan = nhapSoThucKhongAm("Gia Ban: ");

    fs::create_directories(ThuMucCon);
    ofstream file(TenFile);
    // Ghi TenMatHang GOC (co dau cach) vao noi dung file de de doc,
    // du thu muc chua no da duoc chuan hoa bo dau cach.
    file << "Ten Mat Hang: " << TenMatHang << "\n";
    file << "So Seri: " << SoSeri << "\n";
    // fixed + setprecision(0): tranh in gia dang khoa hoc (vd: 2.5e+07)
    // khi gia tri >= 1 trieu, dong nhat voi cach lam o HoaDon::luuFile().
    file << fixed << setprecision(0);
    file << "Gia Nhap: " << GiaNhap << "\n";
    file << "Gia Ban: " << GiaBan << "\n";
    // Trang thai mac dinh khi vua nhap hang: CHUA BAN, chua co khach mua.
    // Se duoc Utils::danhDauMatHangDaBan() cap nhat khi mot hoa don chua
    // so seri nay duoc lap thanh cong.
    file << "Tinh Trang: Chua Ban\n";
    file << "Ma Khach Hang Da Mua: \n";
    file.close();
    cout << "Da luu mat hang thanh cong!\n";
}

// ------------------------------------------------------------
// luuMotMatHang(): ham noi bo, ghi 1 thiet bi ra file theo dung dinh
// dang chuan cua he thong. Tach rieng de NhapHangLoatTuFile() tai su
// dung, dam bao file sinh ra tu "nhap hang loat" GIONG HET file sinh
// ra tu "nhap thu cong" (cung co dong Tinh Trang / Ma Khach Hang Da
// Mua), neu khong cac chuc nang doc kho sau nay se hieu sai du lieu.
// ------------------------------------------------------------
static bool luuMotMatHang(const string& tenMatHang, const string& soSeri,
                           double giaNhap, double giaBan) {
    string thuMucCon = "DataMatHang/" + chuanHoaTenThuMuc(tenMatHang);
    string tenFile = thuMucCon + "/" + soSeri + ".txt";

    fs::create_directories(thuMucCon);
    ofstream file(tenFile);
    if (!file.is_open()) return false;

    file << "Ten Mat Hang: " << tenMatHang << "\n";
    file << "So Seri: " << soSeri << "\n";
    file << fixed << setprecision(0);
    file << "Gia Nhap: " << giaNhap << "\n";
    file << "Gia Ban: " << giaBan << "\n";
    file << "Tinh Trang: Chua Ban\n";
    file << "Ma Khach Hang Da Mua: \n";
    file.close();
    return true;
}

// ------------------------------------------------------------
// tachDong(): tach 1 dong trong file lo hang thanh 4 thanh phan.
// Ho tro 2 dinh dang:
//   - Phan cach bang DAU PHAY : "iphone 15 Pro,a1508,15000000,18000000"
//   - Phan cach bang KHOANG TRANG: "iphone15 a1508 15000000 18000000"
//
// Voi dinh dang khoang trang, ten mat hang co the CHUA khoang trang
// (vd: "iphone 15 pro a1508 15000000 18000000"), nen khong the tach
// don gian tu trai sang. Cach xu ly: lay 3 token CUOI CUNG lam So
// Seri / Gia Nhap / Gia Ban, toan bo phan con lai phia truoc ghep lai
// thanh Ten Mat Hang.
// ------------------------------------------------------------
static bool tachDong(const string& dong, string& tenRa, string& seriRa,
                      double& giaNhapRa, double& giaBanRa) {
    string phanConLai = dong;

    // Bo ky tu '\r' o cuoi dong (file .txt tao tren Windows co CRLF,
    // neu khong bo se lam stod() that bai o truong cuoi cung).
    while (!phanConLai.empty() &&
           (phanConLai.back() == '\r' || phanConLai.back() == '\n')) {
        phanConLai.pop_back();
    }
    if (phanConLai.empty()) return false;

    vector<string> phanTu;

    if (phanConLai.find(',') != string::npos) {
        // ----- Dinh dang phan cach bang dau phay -----
        stringstream ss(phanConLai);
        string o;
        while (getline(ss, o, ',')) {
            size_t dau = o.find_first_not_of(" \t");
            size_t cuoi = o.find_last_not_of(" \t");
            phanTu.push_back(dau == string::npos ? "" : o.substr(dau, cuoi - dau + 1));
        }
        if (phanTu.size() < 4) return false;

        tenRa = phanTu[0];
        seriRa = phanTu[1];
        try {
            giaNhapRa = stod(phanTu[2]);
            giaBanRa = stod(phanTu[3]);
        } catch (...) { return false; }
    } else {
        // ----- Dinh dang phan cach bang khoang trang -----
        stringstream ss(phanConLai);
        string o;
        while (ss >> o) phanTu.push_back(o);
        if (phanTu.size() < 4) return false;

        size_t n = phanTu.size();
        seriRa = phanTu[n - 3];
        try {
            giaNhapRa = stod(phanTu[n - 2]);
            giaBanRa = stod(phanTu[n - 1]);
        } catch (...) { return false; }

        // Ghep tat ca token phia truoc thanh Ten Mat Hang
        tenRa = "";
        for (size_t i = 0; i + 3 < n; ++i) {
            if (!tenRa.empty()) tenRa += " ";
            tenRa += phanTu[i];
        }
    }

    if (tenRa.empty() || seriRa.empty()) return false;
    if (giaNhapRa < 0 || giaBanRa < 0) return false;
    return true;
}

// ------------------------------------------------------------
// NhapHangLoatTuFile(): doc 1 file .txt chua nhieu thiet bi, nhap tat
// ca vao kho trong 1 lan. Bo qua (va bao cao) cac dong bi loi dinh
// dang hoac trung So Seri, thay vi dung han - de 1 dong hong khong
// lam mat toan bo lo hang.
// ------------------------------------------------------------
void NhapHangLoatTuFile() {
    string thuMucNhapHang = "NhapHang";

    // Tu tao thu muc NhapHang neu chua co - de lan chay dau tien nguoi
    // dung khong bi loi "thu muc khong ton tai" ngay lap tuc, ma duoc
    // huong dan bo file vao do roi chay lai.
    fs::create_directories(thuMucNhapHang);

    // Liet ke cac file .txt/.csv dang co trong thu muc NhapHang/ de
    // nguoi dung biet co nhung file nao ma go cho dung ten.
    cout << "\nCac file du lieu dang co trong thu muc \"" << thuMucNhapHang << "\":\n";
    int demFile = 0;
    for (const auto& entry : fs::directory_iterator(thuMucNhapHang)) {
        if (!entry.is_regular_file()) continue;
        string duoi = entry.path().extension().string();
        if (duoi == ".txt" || duoi == ".csv") {
            cout << "  - " << entry.path().filename().string() << "\n";
            demFile++;
        }
    }
    if (demFile == 0) {
        cout << "  (Khong tim thay file .txt/.csv nao - hay bo file lo hang vao thu muc \""
             << thuMucNhapHang << "\" roi thu lai)\n";
        return;
    }

    string tenFile;
    cout << "\nNhap ten file lo hang: ";
    getline(cin >> ws, tenFile);

    // Duong dan day du = NhapHang/<ten file nguoi dung go>. Nguoi dung
    // CHI can go ten file (khong can go "NhapHang/" phia truoc).
    string duongDanFile = thuMucNhapHang + "/" + tenFile;

    if (!fs::exists(duongDanFile)) {
        cout << "Khong tim thay file '" << tenFile << "' trong thu muc \""
             << thuMucNhapHang << "\"!\n";
        return;
    }

    ifstream fileDoc(duongDanFile);
    if (!fileDoc.is_open()) {
        cout << "Khong the mo file '" << tenFile << "'!\n";
        return;
    }

    int soDongThanhCong = 0, soDongLoi = 0, soDongTrungSeri = 0, soThuTuDong = 0;
    string dong;

    cout << "\n================ KET QUA NHAP HANG LOAT ================\n";

    while (getline(fileDoc, dong)) {
        soThuTuDong++;

        // Bo qua dong trong (khong tinh la loi)
        if (dong.find_first_not_of(" \t\r\n") == string::npos) continue;

        string tenMatHang, soSeri;
        double giaNhap = 0, giaBan = 0;

        if (!tachDong(dong, tenMatHang, soSeri, giaNhap, giaBan)) {
            cout << "  [Dong " << soThuTuDong << "] LOI DINH DANG - da bo qua: " << dong << "\n";
            soDongLoi++;
            continue;
        }

        // So Seri phai duy nhat tren TOAN BO kho (giong quy tac cua
        // TaoMatHang) - khong ghi de thiet bi da co san.
        if (soSeriDaTonTai(soSeri)) {
            cout << "  [Dong " << soThuTuDong << "] TRUNG SO SERI '" << soSeri
                 << "' - da bo qua.\n";
            soDongTrungSeri++;
            continue;
        }

        if (luuMotMatHang(tenMatHang, soSeri, giaNhap, giaBan)) {
            cout << "  [Dong " << soThuTuDong << "] OK: " << tenMatHang
                 << " (Seri: " << soSeri << ")\n";
            soDongThanhCong++;
        } else {
            cout << "  [Dong " << soThuTuDong << "] Khong ghi duoc file - da bo qua.\n";
            soDongLoi++;
        }
    }
    fileDoc.close();

    cout << "--------------------------------------------------------\n";
    cout << "Nhap thanh cong : " << soDongThanhCong << " thiet bi\n";
    cout << "Trung so seri   : " << soDongTrungSeri << " dong (bi bo qua)\n";
    cout << "Loi dinh dang   : " << soDongLoi << " dong (bi bo qua)\n";
}