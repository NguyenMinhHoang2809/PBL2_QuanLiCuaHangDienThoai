// ============================================================
// MenuSDL.cpp - Phien ban DO HOA cua chonMenu() dung SDL2 + SDL2_ttf.
//
// File nay CHI duoc bien dich khi co co -DDUNG_SDL (xem BienDich_SDL.bat).
// Khi bien dich binh thuong (g++ *.cpp), toan bo noi dung ben duoi bi
// bo qua -> ban Console khong can SDL2 van chay duoc.
//
// Mo hinh hoat dong (lai giua cua so do hoa va Console):
//   - Menu hien thi tren CUA SO SDL2: nut bam, re chuot doi mau, click.
//   - Sau khi chon, cua so SDL duoc AN di, Console duoc dua len truoc
//     de cac chuc nang nghiep vu (cin/getline) chay NGUYEN VEN nhu cu.
//   - Lan goi chonMenu() ke tiep, cua so SDL hien lai.
// ============================================================
#ifdef DUNG_SDL

// SDL_MAIN_HANDLED: khong de SDL doi ten main() thanh SDL_main -> giu
// nguyen main.cpp, khong can link them SDL2main.
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>

#include "TienIch.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

using namespace std;

namespace {

// ---------------- Cau hinh giao dien ----------------
const char* const TEN_DO_AN = "HOÀNG CỬU BẢO MOBILE";
const int RONG_CUA_SO = 640;
const int RONG_NUT = 460;
const int Y_NUT_DAU = 150;   // toa do y cua nut dau tien (phia tren la tieu de)
const int LE_DUOI = 50;      // khoang trong duoi cung (chua dong goi y)

const SDL_Color MAU_NEN_CUA_SO = { 25, 25, 25, 255 };
const SDL_Color MAU_NUT_THUONG = { 55, 55, 55, 255 };     // xam den
const SDL_Color MAU_NUT_SANG   = { 255, 220, 0, 255 };    // vang ruc
const SDL_Color MAU_TRANG      = { 255, 255, 255, 255 };
const SDL_Color MAU_DEN        = { 0, 0, 0, 255 };
const SDL_Color MAU_TIEU_DE_PHU = { 255, 220, 0, 255 };
const SDL_Color MAU_GOI_Y      = { 150, 150, 150, 255 };

// ---------------- Tai nguyen SDL dung chung ----------------
SDL_Window*   g_cuaSo = nullptr;
SDL_Renderer* g_butVe = nullptr;
TTF_Font*     g_fontTieuDe = nullptr;
TTF_Font*     g_fontTieuDePhu = nullptr;
TTF_Font*     g_fontNut = nullptr;
TTF_Font*     g_fontGoiY = nullptr;
bool g_daThuKhoiTao = false;
bool g_sanSang = false;

// Thu lan luot cac font co san tren Windows, deu ho tro tieng Viet.
TTF_Font* moFont(int coChu, bool dam) {
    const char* dsFontDam[]    = { "C:/Windows/Fonts/segoeuib.ttf", "C:/Windows/Fonts/arialbd.ttf", "C:/Windows/Fonts/tahomabd.ttf" };
    const char* dsFontThuong[] = { "C:/Windows/Fonts/segoeui.ttf",  "C:/Windows/Fonts/arial.ttf",   "C:/Windows/Fonts/tahoma.ttf" };
    const char** ds = dam ? dsFontDam : dsFontThuong;
    for (int i = 0; i < 3; ++i) {
        TTF_Font* f = TTF_OpenFont(ds[i], coChu);
        if (f != nullptr) return f;
    }
    return nullptr;
}

void donDepSDL() {
    TTF_Font** dsFont[] = { &g_fontTieuDe, &g_fontTieuDePhu, &g_fontNut, &g_fontGoiY };
    for (TTF_Font** f : dsFont) {
        if (*f) { TTF_CloseFont(*f); *f = nullptr; }
    }
    if (g_butVe) { SDL_DestroyRenderer(g_butVe); g_butVe = nullptr; }
    if (g_cuaSo) { SDL_DestroyWindow(g_cuaSo); g_cuaSo = nullptr; }
    if (TTF_WasInit()) TTF_Quit();
    SDL_Quit();
}

// Khoi tao SDL MOT LAN DUY NHAT (lan goi chonMenu() dau tien). Neu
// that bai (thieu DLL, khong co font...) -> tra ve false de chonMenu()
// tu dong dung giao dien Console thay the.
bool khoiTaoSDL() {
    if (g_daThuKhoiTao) return g_sanSang;
    g_daThuKhoiTao = true;

    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << "[SDL] Khong khoi tao duoc: " << SDL_GetError() << "\n";
        return false;
    }
    if (TTF_Init() != 0) {
        cerr << "[SDL_ttf] Khong khoi tao duoc: " << TTF_GetError() << "\n";
        SDL_Quit();
        return false;
    }

    g_fontTieuDe    = moFont(30, true);
    g_fontTieuDePhu = moFont(20, true);
    g_fontNut       = moFont(18, false);
    g_fontGoiY      = moFont(14, false);
    if (!g_fontTieuDe || !g_fontTieuDePhu || !g_fontNut || !g_fontGoiY) {
        cerr << "[SDL_ttf] Khong tim thay font chu trong C:/Windows/Fonts\n";
        donDepSDL();
        return false;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    g_cuaSo = SDL_CreateWindow("HOANG CUU BAO MOBILE - PBL2",
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               RONG_CUA_SO, 480, SDL_WINDOW_HIDDEN);
    if (!g_cuaSo) {
        cerr << "[SDL] Khong tao duoc cua so: " << SDL_GetError() << "\n";
        donDepSDL();
        return false;
    }

    g_butVe = SDL_CreateRenderer(g_cuaSo, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!g_butVe) g_butVe = SDL_CreateRenderer(g_cuaSo, -1, SDL_RENDERER_SOFTWARE); // may khong co GPU
    if (!g_butVe) {
        cerr << "[SDL] Khong tao duoc renderer: " << SDL_GetError() << "\n";
        donDepSDL();
        return false;
    }

    atexit(donDepSDL); // Giai phong tai nguyen khi chuong trinh ket thuc
    g_sanSang = true;
    return true;
}

// Chu da duoc "ve san" thanh texture (tranh render lai font moi khung hinh)
struct VanBan {
    SDL_Texture* texture = nullptr;
    int rong = 0, cao = 0;
};

VanBan taoVanBan(TTF_Font* font, const string& noiDung, SDL_Color mau) {
    VanBan vb;
    if (noiDung.empty()) return vb;
    SDL_Surface* be = TTF_RenderUTF8_Blended(font, noiDung.c_str(), mau); // UTF-8 -> tieng Viet co dau
    if (!be) return vb;
    vb.texture = SDL_CreateTextureFromSurface(g_butVe, be);
    vb.rong = be->w;
    vb.cao = be->h;
    SDL_FreeSurface(be);
    return vb;
}

void huyVanBan(VanBan& vb) {
    if (vb.texture) SDL_DestroyTexture(vb.texture);
    vb.texture = nullptr;
}

void veCanGiua(const VanBan& vb, int tamX, int tamY) {
    if (!vb.texture) return;
    SDL_Rect viTri = { tamX - vb.rong / 2, tamY - vb.cao / 2, vb.rong, vb.cao };
    SDL_RenderCopy(g_butVe, vb.texture, nullptr, &viTri);
}

void datMau(SDL_Color m) {
    SDL_SetRenderDrawColor(g_butVe, m.r, m.g, m.b, m.a);
}

} // namespace

int chonMenu(const string& tieuDe, const vector<string>& cacMuc) {
    const int soMuc = static_cast<int>(cacMuc.size());
    if (soMuc == 0) return -1;
    if (!khoiTaoSDL()) return chonMenuConsole(tieuDe, cacMuc); // Du phong

    // Console chi hien loi nhan trong luc menu o cua so do hoa
    clearScreen();
    cout << "\n  >> Menu \"" << tieuDe << "\" dang hien thi o cua so do hoa...\n" << flush;

    // ---------- Bo cuc: cac nut xep doc, can giua ----------
    int caoNut = 46, khoangCach = 12;
    SDL_Rect vungManHinh;
    int caoCuaSo = Y_NUT_DAU + soMuc * caoNut + (soMuc - 1) * khoangCach + LE_DUOI;
    if (SDL_GetDisplayUsableBounds(0, &vungManHinh) == 0 && caoCuaSo > vungManHinh.h - 40) {
        caoNut = 36; khoangCach = 6; // Man hinh thap -> thu gon nut
        caoCuaSo = Y_NUT_DAU + soMuc * caoNut + (soMuc - 1) * khoangCach + LE_DUOI;
    }

    vector<SDL_Rect> cacNut(soMuc);
    for (int i = 0; i < soMuc; ++i) {
        cacNut[i] = { (RONG_CUA_SO - RONG_NUT) / 2, Y_NUT_DAU + i * (caoNut + khoangCach), RONG_NUT, caoNut };
    }

    SDL_SetWindowSize(g_cuaSo, RONG_CUA_SO, caoCuaSo);
    SDL_SetWindowPosition(g_cuaSo, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(g_cuaSo);
    SDL_RaiseWindow(g_cuaSo);

    // ---------- Tao san texture chu (moi nut 2 ban: chu trang / chu den) ----------
    VanBan vbTenDoAn = taoVanBan(g_fontTieuDe, TEN_DO_AN, MAU_TRANG);
    VanBan vbTieuDe  = taoVanBan(g_fontTieuDePhu, tieuDe, MAU_TIEU_DE_PHU);
    vector<VanBan> vbThuong(soMuc), vbSang(soMuc);
    for (int i = 0; i < soMuc; ++i) {
        vbThuong[i] = taoVanBan(g_fontNut, cacMuc[i], MAU_TRANG);
        vbSang[i]   = taoVanBan(g_fontNut, cacMuc[i], MAU_DEN);
    }

    auto timNut = [&](int x, int y) -> int {
        SDL_Point diem = { x, y };
        for (int i = 0; i < soMuc; ++i) {
            if (SDL_PointInRect(&diem, &cacNut[i])) return i;
        }
        return -1;
    };

    // Neu chuot DANG nam tren 1 nut ngay luc mo menu -> sang nut do luon
    int chuotX = 0, chuotY = 0;
    SDL_GetMouseState(&chuotX, &chuotY);
    int dangChon = timNut(chuotX, chuotY);   // -1 = khong nut nao sang

    // Ve ca khung hinh vao bo dem sau roi SDL_RenderPresent mot lan ->
    // khong nhap nhay (double buffering cua SDL renderer).
    auto veManHinh = [&]() {
        datMau(MAU_NEN_CUA_SO);
        SDL_RenderClear(g_butVe);

        veCanGiua(vbTenDoAn, RONG_CUA_SO / 2, 50);
        veCanGiua(vbTieuDe, RONG_CUA_SO / 2, 98);

        for (int i = 0; i < soMuc; ++i) {
            bool sang = (i == dangChon);
            datMau(sang ? MAU_NUT_SANG : MAU_NUT_THUONG);
            SDL_RenderFillRect(g_butVe, &cacNut[i]);
            datMau(MAU_TRANG);                       // vien manh mau trang
            SDL_RenderDrawRect(g_butVe, &cacNut[i]);
            veCanGiua(sang ? vbSang[i] : vbThuong[i],
                      cacNut[i].x + cacNut[i].w / 2, cacNut[i].y + cacNut[i].h / 2);
        }

        SDL_RenderPresent(g_butVe);
    };

    // ---------- Vong lap su kien ----------
    int ketQua = -1;
    bool canVeLai = true;
    while (ketQua < 0) {
        if (canVeLai) { veManHinh(); canVeLai = false; }

        SDL_Event e;
        if (!SDL_WaitEvent(&e)) continue;

        switch (e.type) {
            case SDL_QUIT:
                ketQua = soMuc - 1; // Dong cua so = chon muc cuoi (thoat/quay lai)
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_CLOSE) ketQua = soMuc - 1;
                else if (e.window.event == SDL_WINDOWEVENT_EXPOSED) canVeLai = true;
                break;
            case SDL_MOUSEMOTION: {
                int viTri = timNut(e.motion.x, e.motion.y);
                if (viTri != dangChon) { dangChon = viTri; canVeLai = true; }
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
                if (e.button.button == SDL_BUTTON_LEFT) {
                    int viTri = timNut(e.button.x, e.button.y);
                    if (viTri >= 0) ketQua = viTri;
                }
                break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        dangChon = (dangChon <= 0) ? soMuc - 1 : dangChon - 1;
                        canVeLai = true;
                        break;
                    case SDLK_DOWN:
                        dangChon = (dangChon < 0 || dangChon >= soMuc - 1) ? 0 : dangChon + 1;
                        canVeLai = true;
                        break;
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER:
                        if (dangChon >= 0) ketQua = dangChon;
                        break;
                    case SDLK_ESCAPE:
                        ketQua = soMuc - 1;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    // ---------- Don dep + tra quyen dieu khien ve Console ----------
    huyVanBan(vbTenDoAn);
    huyVanBan(vbTieuDe);
    for (int i = 0; i < soMuc; ++i) {
        huyVanBan(vbThuong[i]);
        huyVanBan(vbSang[i]);
    }

    SDL_HideWindow(g_cuaSo);
    SDL_PumpEvents();

#ifdef _WIN32
    // Dua cua so Console len truoc de nguoi dung go phim nhap lieu ngay,
    // va xoa moi phim da lo go vao Console trong luc dang o menu SDL.
    HWND hConsole = GetConsoleWindow();
    if (hConsole) {
        ShowWindow(hConsole, SW_RESTORE);
        SetForegroundWindow(hConsole);
    }
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
#endif
    clearScreen();

    return ketQua;
}

#endif // DUNG_SDL

