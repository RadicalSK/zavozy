#ifndef ZAVOZ_H
#define ZAVOZ_H

#include <QJsonObject>
#include <QRegularExpression>
#include <QString>
#include "Pacient.h"
#include "PolozkaZavozu.h"

class Zavoz
{
public:
    Zavoz();
    Zavoz(Pacient *aktualniPacient, QString aktualniPoznamka, QList<PolozkaZavozu> seznamPolozek);

    QString prijmeni() const;
    QString jmeno() const;
    QString celeJmeno() const;
    QString ulice() const;
    QString obec() const;
    QString adresaDvojradkove() const;
    QString telefony() const;
    QString poznamka() const;
    QList<PolozkaZavozu> polozky() const;

    void zapsatJson(QJsonObject &json) const;
    void cistJson(const QJsonObject &json);

private:
    QString prijmeni_;
    QString jmeno_;
    QString ulice_;
    QString obec_;
    QString telefony_;
    QString poznamka_;
    QList<PolozkaZavozu> polozky_;
};

#endif // ZAVOZ_H
