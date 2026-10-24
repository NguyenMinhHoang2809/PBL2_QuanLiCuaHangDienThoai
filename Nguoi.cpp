#include "Nguoi.h"

Nguoi::Nguoi() : ID(""), matKhau(""), hoTen("") {}

Nguoi::Nguoi(const std::string& id, const std::string& mk, const std::string& ten)
    : ID(id), matKhau(mk), hoTen(ten) {}

Nguoi::~Nguoi() {
    // Khong co tai nguyen dong nao can giai phong thu cong o lop nay.
    // Destructor van duoc khai bao virtual de dam bao an toan da hinh.
}

std::string Nguoi::getID() const {
    return ID;
}

std::string Nguoi::getMatKhau() const {
    return matKhau;
}

std::string Nguoi::getHoTen() const {
    return hoTen;
}

void Nguoi::setID(const std::string& id) {
    ID = id;
}

void Nguoi::setMatKhau(const std::string& mk) {
    matKhau = mk;
}

void Nguoi::setHoTen(const std::string& ten) {
    hoTen = ten;
}



