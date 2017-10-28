#include "Pacient.h"

Pacient::Pacient(int id, QString prijmeni, QString jmeno, QString ulice,
                 QString obec, QString telefon1, QString poznTel1,
                 QString telefon2, QString poznTel2, QString poznamka) :
    id_(id), prijmeni_(prijmeni), jmeno_(jmeno), ulice_(ulice),
    obec_(obec), telefon1_(telefon1), poznTel1_(poznTel1),
    telefon2_(telefon2), poznTel2_(poznTel2), poznamka_(poznamka) {

}
int Pacient::id() const {
    return id_;
}

QString Pacient::prijmeni() const {
    return prijmeni_;
}

QString Pacient::jmeno() const {
    return jmeno_;
}

QString Pacient::ulice() const {
    return ulice_;
}

QString Pacient::obec() const {
    return obec_;
}

QString Pacient::adresa() const {
    return QString(ulice_ + ", " + obec_);
}

QString Pacient::telefon1() const {
    return telefon1_;
}

QString Pacient::poznTel1() const {
    return poznTel1_;
}

QString Pacient::telefon2() const {
    return telefon2_;
}

QString Pacient::poznTel2() const {
    return poznTel2_;
}

QString Pacient::telefony() const {
    QRegularExpression trojiceCislicRegExp("(\\d{3})");
    QRegularExpression celyRetezecRegExp("(^.*$)");

    QString telefony = QString(telefon1_).replace(trojiceCislicRegExp, "\\1 ");

    if (!poznTel1_.isEmpty())
        telefony += QString(poznTel1_).replace(celyRetezecRegExp, "(\\1)");

    if (!telefon2_.isEmpty())
        telefony += QString(", " + QString(telefon2_).replace(trojiceCislicRegExp, "\\1 "));

    if (!poznTel2_.isEmpty())
        telefony += QString(poznTel2_).replace(celyRetezecRegExp, "(\\1)");

    return telefony.trimmed();
}

QString Pacient::poznamka() const {
    return poznamka_;
}

void Pacient::setId(int id) {
    id_ = id;
}

void Pacient::setPrijmeni(const QString &prijmeni) {
    prijmeni_ = prijmeni;
}

void Pacient::setJmeno(const QString &jmeno) {
    jmeno_ = jmeno;
}

void Pacient::setUlice(const QString &ulice) {
    ulice_ = ulice;
}

void Pacient::setObec(const QString &obec) {
    obec_ = obec;
}

void Pacient::setTelefon1(const QString &telefon1) {
    telefon1_ = telefon1;
}

void Pacient::setPoznTel1(const QString &poznTel1) {
    poznTel1_ = poznTel1;
}

void Pacient::setTelefon2(const QString &telefon2) {
    telefon2_ = telefon2;
}

void Pacient::setPoznTel2(const QString &poznTel2) {
    poznTel2_ = poznTel2;
}

void Pacient::setPoznamka(const QString &poznamka) {
    poznamka_ = poznamka;
}
