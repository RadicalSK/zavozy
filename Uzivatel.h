#ifndef UZIVATEL_H
#define UZIVATEL_H

#include <QString>
#include "globalni.h"

class Uzivatel
{
public:
    Uzivatel(QString uzivJmeno, QString hesloSha256Hash, QString celeJmeno, bool admin);

    QString uzivJmeno() const;
    QString hesloSha256Hash() const;
    QString celeJmeno() const;
    bool admin() const;

    void setUzivJmeno(const QString &uzivJmeno);
    void setHesloSha256Hash(const QString &noveHeslo);
    void setCeleJmeno(const QString &celeJmeno);
    void setAdmin(bool admin);

private:
    QString uzivJmeno_;
    QString hesloSha256Hash_;
    QString celeJmeno_;
    bool admin_ = false;
};

#endif // UZIVATEL_H
