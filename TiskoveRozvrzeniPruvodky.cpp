#include "TiskoveRozvrzeniPruvodky.h"

TiskoveRozvrzeniPruvodky::TiskoveRozvrzeniPruvodky(const QRect &obdelnik, QPainter *painter, bool jePoznamka)
    : obdelnik_(obdelnik), painter_(painter), jePoznamka_(jePoznamka)
{
    // zjisti vysky pisem
    painter_->setFont(TiskoveFonty::font(TiskoveFonty::Normalni));
    QFontMetrics metrikaNormalni(painter_->fontMetrics());
    painter_->setFont(TiskoveFonty::font(TiskoveFonty::Popisek));
    QFontMetrics metrikaPopisek(painter_->fontMetrics());
    painter_->setFont(TiskoveFonty::font(TiskoveFonty::Jmeno));
    QFontMetrics metrikaJmeno(painter_->fontMetrics());

    vyskyFontu.insert(TiskoveFonty::Normalni, metrikaNormalni.height());
    vyskyFontu.insert(TiskoveFonty::Popisek, metrikaPopisek.height());
    vyskyFontu.insert(TiskoveFonty::Jmeno, metrikaJmeno.height());

    // vytvori geometrii pro jmeno, adresu a telefon
    jmeno_ = radekPruvodky(TiskoveFonty::Jmeno);
    adresa_ = radekPruvodky(TiskoveFonty::Normalni, 2);
    telefon_ = radekPruvodky(TiskoveFonty::Normalni);

    // vytvori geometrii pro seznam pomucek, ev. poznamku
    int vyskaRamuRadkuPomucek = vypocitatVyskuRamu(vyskyFontu[TiskoveFonty::Normalni]);
    maxPocetRadkuPomucek_ = (obdelnik_.height() - zabranaVyska_) / vyskaRamuRadkuPomucek - (int)jePoznamka;

    for (int i = 0; i < maxPocetRadkuPomucek_; i++) {
        radkyPomucek_.append(radekPruvodky(TiskoveFonty::Normalni));
    }

    if (jePoznamka)
        poznamka_ = radekPruvodky(TiskoveFonty::Normalni);

    // vytvori vektory obdelniku popisku
    obdelnikyPopisku_ << jmeno_.textPopisek << adresa_.textPopisek << telefon_.textPopisek
                      << radkyPomucek_.at(0).textPopisek << poznamka_.textPopisek;

    // vytvori vektor obdelniku pomucek
    foreach (RadekPruvodky radek, radkyPomucek_) {
        obdelnikyPomucek_ << radek.textObsah;
    }
}

QVector<QRect> TiskoveRozvrzeniPruvodky::rameckyKTisku() const {
    return rameckyKTisku_;
}

QVector<QRect> TiskoveRozvrzeniPruvodky::obdelnikyPopisku() const {
    return obdelnikyPopisku_;
}

QRect TiskoveRozvrzeniPruvodky::obdelnikObsahuJmeno() const {
    return jmeno_.textObsah;
}

QRect TiskoveRozvrzeniPruvodky::obdelnikObsahuAdresa() const {
    return adresa_.textObsah;
}

QRect TiskoveRozvrzeniPruvodky::obdelnikObsahuTelefon() const {
    return telefon_.textObsah;
}

QRect TiskoveRozvrzeniPruvodky::obdelnikObsahuPoznamka() const {
    return poznamka_.textObsah;
}

QVector<QRect> TiskoveRozvrzeniPruvodky::obdelnikyPomucek() const {
    return obdelnikyPomucek_;
}

int TiskoveRozvrzeniPruvodky::maxPocetRadkuPomucek() const {
    return maxPocetRadkuPomucek_;
}

QString TiskoveRozvrzeniPruvodky::zkratitTextRadkuPomucky(QString celyRadek) const {
    painter_->setFont(TiskoveFonty::font(TiskoveFonty::Normalni));
    QFontMetrics metrika(painter_->fontMetrics());
    return metrika.elidedText(celyRadek, Qt::ElideRight, obdelnikyPomucek_.at(0).width());
}

TiskoveRozvrzeniPruvodky::RadekPruvodky TiskoveRozvrzeniPruvodky::radekPruvodky(TiskoveFonty::Styl fontObsahu,
                                                                                int pocetRadku) {
    RadekPruvodky radek;
    radek.ramecekPopisek = vytvoritRamecek(Popisek, fontObsahu, pocetRadku);
    radek.ramecekObsah = vytvoritRamecek(Obsah, fontObsahu, pocetRadku);
    radek.textPopisek = vytvoritTextovyObdelnik(Popisek, radek.ramecekPopisek);
    radek.textObsah = vytvoritTextovyObdelnik(Obsah, radek.ramecekObsah);

    rameckyKTisku_ << radek.ramecekPopisek << radek.ramecekObsah;

    zabranaVyska_ += radek.ramecekPopisek.height();

    return radek;
}

QRect TiskoveRozvrzeniPruvodky::vytvoritRamecek(TiskoveRozvrzeniPruvodky::CastRadku cast,
                                                TiskoveFonty::Styl fontObsahu, int pocetRadku) {
    pocetRadku = qMax(1, pocetRadku);

    int x = obdelnik_.x();
    int y = obdelnik_.y();
    int w = obdelnik_.width();
    int h = vypocitatVyskuRamu(vyskyFontu[fontObsahu], pocetRadku);
    int wPopisek = w * relSirkaPopisku_;
    int wObsah = w - wPopisek;

    switch (cast) {
    case Popisek:
        return QRect(x, y + zabranaVyska_, wPopisek, h);
    case Obsah:
    default:
        return QRect(x + wPopisek, y + zabranaVyska_, wObsah, h);
    }
}

QRect TiskoveRozvrzeniPruvodky::vytvoritTextovyObdelnik(TiskoveRozvrzeniPruvodky::CastRadku cast, QRect ramecek) {
    int padding = (vyskyFontu[TiskoveFonty::Normalni] * (radkovani_ - 1)) / 2;

    switch (cast) {
    case Popisek:
        return ramecek.adjusted(padding, ramecek.height() - padding - vyskyFontu[TiskoveFonty::Popisek],
                                           -padding, -padding);
    case Obsah:
    default:
        return ramecek.adjusted(padding, padding,
                                        -padding, -padding);
    }
}

int TiskoveRozvrzeniPruvodky::vypocitatVyskuRamu(int vyskaPisma, int pocetRadku) {
    return vyskaPisma * pocetRadku + vyskyFontu[TiskoveFonty::Normalni] * (radkovani_ - 1);
}
