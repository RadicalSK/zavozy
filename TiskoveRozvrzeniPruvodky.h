#ifndef TISKOVEROZVRZENIPRUVODKY_H
#define TISKOVEROZVRZENIPRUVODKY_H

#include <QFontMetrics>
#include <QMap>
#include <QPainter>
#include <QRect>
#include "TiskoveFonty.h"

class TiskoveRozvrzeniPruvodky
{
public:
    TiskoveRozvrzeniPruvodky(const QRect &obdelnik, QPainter *painter, bool jePoznamka);

    QVector<QRect> rameckyKTisku() const;
    QVector<QRect> obdelnikyPopisku() const;
    QRect obdelnikObsahuJmeno() const;
    QRect obdelnikObsahuAdresa() const;
    QRect obdelnikObsahuTelefon() const;
    QRect obdelnikObsahuPoznamka() const;
    QVector<QRect> obdelnikyPomucek() const;
    int maxPocetRadkuPomucek() const;
    QString zkratitTextRadkuPomucky(QString celyRadek) const;

private:
    QMap<TiskoveFonty::Styl, int> vyskyFontu;

    enum CastRadku {
        Popisek, Obsah
    };

    struct RadekPruvodky {
        QRect ramecekPopisek;
        QRect ramecekObsah;
        QRect textPopisek;
        QRect textObsah;
    };

    RadekPruvodky radekPruvodky(TiskoveFonty::Styl fontObsahu, int pocetRadku = 1);
    QRect vytvoritRamecek(CastRadku cast, TiskoveFonty::Styl fontObsahu, int pocetRadku = 1);
    QRect vytvoritTextovyObdelnik(CastRadku cast, QRect ramecek);
    int vypocitatVyskuRamu(int vyskaPisma, int pocetRadku = 1);

    QRect obdelnik_;
    QPainter *painter_;
    bool jePoznamka_;

    const float radkovani_ = 1.8;
    const float relSirkaPopisku_ = 0.12;

    int zabranaVyska_ = 0;
    int pocetRadkuPomucek_ = 0;
    int maxPocetRadkuPomucek_;

    RadekPruvodky jmeno_, adresa_, telefon_, poznamka_;
    QVector<RadekPruvodky> radkyPomucek_;
    QVector<QRect> rameckyKTisku_, obdelnikyPopisku_, obdelnikyPomucek_;
};

#endif // TISKOVEROZVRZENIPRUVODKY_H
