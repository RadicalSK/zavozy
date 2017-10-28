#ifndef POLOZKAZAVOZU_H
#define POLOZKAZAVOZU_H

#include <QJsonObject>
#include <QString>
#include "Databaze.h"

class PolozkaZavozu
{
public:
    PolozkaZavozu();
    PolozkaZavozu(int pocet, int kodVzp, QString nazev, QString doplnek);

    int pocet() const;
    int kodVzp() const;
    QString nazev() const;
    QString doplnek() const;
    QString celyNazevJednoradkove() const;
    QString celyNazevDvouradkove() const;

    void zapsatJson(QJsonObject &json) const;
    void cistJson(const QJsonObject &json);

private:
    int pocet_;
    int kodVzp_;
    QString nazev_;
    QString doplnek_;
};

#endif // POLOZKAZAVOZU_H
