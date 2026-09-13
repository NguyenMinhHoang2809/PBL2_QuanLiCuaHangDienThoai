#include "Nguoi.h"

Nguoi::Nguoi() : ID(""), matKhau("") {}

Nguoi::Nguoi(const std::string& id, const std::string& mk) : ID(id), matKhau(mk) {}

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

void Nguoi::setID(const std::string& id) {
    ID = id;
}

void Nguoi::setMatKhau(const std::string& mk) {
    matKhau = mk;
}