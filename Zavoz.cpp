#include "Zavoz.h"

Zavoz::Zavoz() {
}

Zavoz::Zavoz(Pacient *aktualniPacient, QString aktualniPoznamka, QList<PolozkaZavozu> seznamPolozek)
    : prijmeni_(aktualniPacient->prijmeni()),
      jmeno_(QString(aktualniPacient->jmeno()).replace(QRegularExpression("\\s+\\(?\\d+\\)?$"), "")),
      ulice_(aktualniPacient->ulice()), obec_(aktualniPacient->obec()),
      telefony_(aktualniPacient->telefony()), poznamka_(aktualniPoznamka), polozky_(seznamPolozek)
{}

QString Zavoz::prijmeni() const {
    return prijmeni_;
}

QString Zavoz::jmeno() const {
    return jmeno_;
}

QString Zavoz::celeJmeno() const {
    return QString(prijmeni_ + " " + jmeno_);
}

QString Zavoz::ulice() const {
    return ulice_;
}

QString Zavoz::obec() const {
    return obec_;
}

QString Zavoz::adresaDvojradkove() const {
    QRegularExpression nulovePsc("^0{5} ?");

    return QString(ulice_ + "\n" + QString(obec_).replace(nulovePsc, ""));
}

QString Zavoz::telefony() const {
    return telefony_;
}

QString Zavoz::poznamka() const {
    return poznamka_;
}

QList<PolozkaZavozu> Zavoz::polozky() const {
    return polozky_;
}

void Zavoz::zapsatJson(QJsonObject &json) const {
    json["prijmeni"] = prijmeni_;
    json["jmeno"] = jmeno_;
    json["ulice"] = ulice_;
    json["obec"] = obec_;
    json["telefony"] = telefony_;
    json["poznamka"] = poznamka_;

    QJsonArray polePolozek;
    foreach (const PolozkaZavozu polozka, polozky_) {
        QJsonObject objektPolozka;
        polozka.zapsatJson(objektPolozka);
        polePolozek.append(objektPolozka);
    }
    json["polozky"] = polePolozek;
}

void Zavoz::cistJson(const QJsonObject &json) {
    prijmeni_ = json["prijmeni"].toString();
    jmeno_ = json["jmeno"].toString();
    ulice_ = json["ulice"].toString();
    obec_ = json["obec"].toString();
    telefony_ = json["telefony"].toString();
    poznamka_ = json["poznamka"].toString();

    polozky_.clear();
    QJsonArray polePolozek = json["polozky"].toArray();
    for (int i = 0; i < polePolozek.size(); i++) {
        QJsonObject objektPolozka = polePolozek[i].toObject();
        PolozkaZavozu polozka;
        polozka.cistJson(objektPolozka);
        polozky_.append(polozka);
    }
}
