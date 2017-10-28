#ifndef PACIENT_H
#define PACIENT_H

#include <QRegularExpression>
#include <QString>

class Pacient
{
public:
    Pacient(int id, QString prijmeni, QString jmeno, QString ulice,
            QString obec, QString telefon1, QString poznTel1 = "",
            QString telefon2 = "", QString poznTel2 = "", QString poznamka = "");

    int id() const;
    QString prijmeni() const;
    QString jmeno() const;
    QString ulice() const;
    QString obec() const;
    QString adresa() const;
    QString telefon1() const;
    QString poznTel1() const;
    QString telefon2() const;
    QString poznTel2() const;
    QString telefony() const;
    QString poznamka() const;

    void setId(int id);
    void setPrijmeni(const QString &prijmeni);
    void setJmeno(const QString &jmeno);
    void setUlice(const QString &ulice);
    void setObec(const QString &obec);
    void setTelefon1(const QString &telefon1);
    void setPoznTel1(const QString &poznTel1);
    void setTelefon2(const QString &telefon2);
    void setPoznTel2(const QString &poznTel2);
    void setPoznamka(const QString &poznamka);

private:
    int id_;
    QString prijmeni_;
    QString jmeno_;
    QString ulice_;
    QString obec_;
    QString telefon1_;
    QString poznTel1_;
    QString telefon2_;
    QString poznTel2_;
    QString poznamka_;
};

#endif // PACIENT_H
