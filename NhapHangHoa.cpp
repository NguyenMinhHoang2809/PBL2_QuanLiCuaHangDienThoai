#include "NhapHangHoa.h"
#include "TienIch.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

// ------------------------------------------------------------
// TaoMatHang: Hien thi khung Form phang can giua man hinh voi tieu de
// "NHẬP HÀNG HÓA MỚI", nhan tieng Viet co dau thang hang doc.
// ------------------------------------------------------------
void TaoMatHang() {
    clearScreen();
    veTieuDeForm("NHẬP HÀNG HÓA MỚI",
                 "(Điền đầy đủ thông tin thiết bị để lưu vào kho)",
                 58);

    string TenMatHang = nhapChuoiForm("Nhập tên mặt hàng", "", false, 58, 20);
    string SoSeri     = nhapChuoiForm("Số seri", "", false, 58, 20);

    string ThuMucGoc = "DataMatHang";
    string ThuMucCon = ThuMucGoc + "/" + chuanHoaTenThuMuc(TenMatHang);
    string TenFile = ThuMucCon + "/" + SoSeri + ".txt";

    // Kiem tra So Seri TRUNG LAP tren TOAN BO DataMatHang
    if (soSeriDaTonTai(SoSeri)) {
        cout << "\n";
        inThongBaoForm("Số seri này đã tồn tại! Số seri phải là duy nhất trên toàn hệ thống.", LOAI_LOI);
        return;
    }

    double GiaNhap = nhapSoThucKhongAm("Giá nhập (VNĐ)");
    double GiaBan  = nhapSoThucKhongAm("Giá bán (VNĐ)");

    fs::create_directories(ThuMucCon);
    ofstream file(TenFile);
    file << "Ten Mat Hang: " << TenMatHang << "\n";
    file << "So Seri: " << SoSeri << "\n";
    file << fixed << setprecision(0);
    file << "Gia Nhap: " << GiaNhap << "\n";
    file << "Gia Ban: " << GiaBan << "\n";
    file << "Tinh Trang: Chua Ban\n";
    file << "Ma Khach Hang Da Mua: \n";
    file.close();

    cout << "\n";
    inThongBaoForm("Đã lưu mặt hàng thành công!", LOAI_THANH_CONG);
}

// ------------------------------------------------------------
// luuMotMatHang(): ham noi bo, ghi 1 thiet bi ra file theo dung dinh
// dang chuan cua he thong.
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
// ------------------------------------------------------------
static bool tachDong(const string& dong, string& tenRa, string& seriRa,
                      double& giaNhapRa, double& giaBanRa) {
    string phanConLai = dong;

    while (!phanConLai.empty() &&
           (phanConLai.back() == '\r' || phanConLai.back() == '\n')) {
        phanConLai.pop_back();
    }
    if (phanConLai.empty()) return false;

    vector<string> phanTu;

    if (phanConLai.find(',') != string::npos) {
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
// NhapHangLoatTuFile(): doc 1 file .txt/.csv chua nhieu thiet bi, nhap
// tat ca vao kho trong 1 lan voi giao dien Form phang can giua.
// ------------------------------------------------------------
void NhapHangLoatTuFile() {
    clearScreen();
    string thuMucNhapHang = "NhapHang";
    fs::create_directories(thuMucNhapHang);

    veTieuDeForm("NHẬP HÀNG LOẠT TỪ FILE",
                 "(Đặt file .txt / .csv vào thư mục \"NhapHang\" trước khi nhập)",
                 58);

    inDongTrongKhungForm("Các file dữ liệu hiện có trong thư mục \"" + thuMucNhapHang + "\":",
                         58, "\033[48;5;236m\033[93;1m");

    int demFile = 0;
    for (const auto& entry : fs::directory_iterator(thuMucNhapHang)) {
        if (!entry.is_regular_file()) continue;
        string duoi = entry.path().extension().string();
        if (duoi == ".txt" || duoi == ".csv") {
            inDongTrongKhungForm("  • " + entry.path().filename().string(), 58);
            demFile++;
        }
    }
    if (demFile == 0) {
        cout << "\n";
        inThongBaoForm("Không tìm thấy file .txt/.csv nào trong thư mục \"" + thuMucNhapHang + "\"!", LOAI_LOI);
        return;
    }

    cout << "\n";
    string tenFile = nhapChuoiForm("Nhập tên file lô hàng", "", false, 58, 22);
    string duongDanFile = thuMucNhapHang + "/" + tenFile;

    if (!fs::exists(duongDanFile)) {
        cout << "\n";
        inThongBaoForm("Không tìm thấy file '" + tenFile + "' trong thư mục \"" + thuMucNhapHang + "\"!", LOAI_LOI);
        return;
    }

    ifstream fileDoc(duongDanFile);
    if (!fileDoc.is_open()) {
        cout << "\n";
        inThongBaoForm("Không thể mở file '" + tenFile + "'!", LOAI_LOI);
        return;
    }

    int soDongThanhCong = 0, soDongLoi = 0, soDongTrungSeri = 0, soThuTuDong = 0;
    string dong;

    cout << "\n";
    inDongTrongKhungForm("KẾT QUẢ NHẬP HÀNG LOẠT", 58, "\033[48;5;236m\033[93;1m");

    while (getline(fileDoc, dong)) {
        soThuTuDong++;

        if (dong.find_first_not_of(" \t\r\n") == string::npos) continue;

        string tenMatHang, soSeri;
        double giaNhap = 0, giaBan = 0;

        if (!tachDong(dong, tenMatHang, soSeri, giaNhap, giaBan)) {
            inDongTrongKhungForm("[Dòng " + to_string(soThuTuDong) + "] LỖI ĐỊNH DẠNG: " + dong,
                                 58, "\033[48;5;236m\033[91m");
            soDongLoi++;
            continue;
        }

        if (soSeriDaTonTai(soSeri)) {
            inDongTrongKhungForm("[Dòng " + to_string(soThuTuDong) + "] TRÙNG SỐ SERI '" + soSeri + "' - bỏ qua",
                                 58, "\033[48;5;236m\033[93m");
            soDongTrungSeri++;
            continue;
        }

        if (luuMotMatHang(tenMatHang, soSeri, giaNhap, giaBan)) {
            inDongTrongKhungForm("[Dòng " + to_string(soThuTuDong) + "] OK: " + tenMatHang + " (Seri: " + soSeri + ")",
                                 58, "\033[48;5;236m\033[92m");
            soDongThanhCong++;
        } else {
            inDongTrongKhungForm("[Dòng " + to_string(soThuTuDong) + "] Không ghi được file - bỏ qua",
                                 58, "\033[48;5;236m\033[91m");
            soDongLoi++;
        }
    }
    fileDoc.close();

    cout << "\n";
    inTruongDaNhapForm("Nhập thành công", to_string(soDongThanhCong) + " thiết bị", 58, 20);
    inTruongDaNhapForm("Trùng số seri",   to_string(soDongTrungSeri) + " dòng (bỏ qua)", 58, 20);
    inTruongDaNhapForm("Lỗi định dạng",   to_string(soDongLoi) + " dòng (bỏ qua)", 58, 20);
}
