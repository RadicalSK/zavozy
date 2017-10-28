#include "Uzivatel.h"

Uzivatel::Uzivatel(QString uzivJmeno, QString hesloSha256Hash, QString celeJmeno, bool admin)
    : uzivJmeno_(uzivJmeno), hesloSha256Hash_(hesloSha256Hash), celeJmeno_(celeJmeno), admin_(admin) {

}

QString Uzivatel::uzivJmeno() const {
    return uzivJmeno_;
}

QString Uzivatel::hesloSha256Hash() const {
    return hesloSha256Hash_;
}

QString Uzivatel::celeJmeno() const {
    return celeJmeno_;
}

bool Uzivatel::admin() const {
    return admin_;
}

void Uzivatel::setUzivJmeno(const QString &uzivJmeno) {
    uzivJmeno_ = uzivJmeno;
}

void Uzivatel::setHesloSha256Hash(const QString &noveHeslo) {
    hesloSha256Hash_ = Heslo::sha256Hash(noveHeslo);
}

void Uzivatel::setCeleJmeno(const QString &celeJmeno) {
    celeJmeno_ = celeJmeno;
}

void Uzivatel::setAdmin(bool admin) {
    admin_ = admin;
}
