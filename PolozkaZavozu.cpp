#include "PolozkaZavozu.h"

PolozkaZavozu::PolozkaZavozu() {
}

PolozkaZavozu::PolozkaZavozu(int pocet, int kodVzp, QString nazev, QString doplnek)
    : pocet_(pocet), kodVzp_(kodVzp), nazev_(nazev), doplnek_(doplnek)
{}

int PolozkaZavozu::pocet() const {
    return pocet_;
}

int PolozkaZavozu::kodVzp() const {
    return kodVzp_;
}

QString PolozkaZavozu::nazev() const {
    return nazev_;
}

QString PolozkaZavozu::doplnek() const {
    return doplnek_;
}

QString PolozkaZavozu::celyNazevJednoradkove() const {
    QString celyNazev = nazev_;
    if (!doplnek_.isEmpty())
        celyNazev += QString(", " + doplnek_);
    return celyNazev;
}

QString PolozkaZavozu::celyNazevDvouradkove() const {
    return QString(nazev_ + "\n" + doplnek_);
}

void PolozkaZavozu::zapsatJson(QJsonObject &json) const {
    json["pocet"] = pocet_;
    json["kodVzp"] = kodVzp_;
    json["nazev"] = nazev_;
    json["doplnek"] = doplnek_;
}

void PolozkaZavozu::cistJson(const QJsonObject &json) {
    pocet_ = json["pocet"].toInt();
    kodVzp_ = json["kodVzp"].toInt();
    nazev_ = json["nazev"].toString();
    doplnek_ = json["doplnek"].toString();
}
