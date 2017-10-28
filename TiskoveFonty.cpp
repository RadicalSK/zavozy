#include "TiskoveFonty.h"

QFont TiskoveFonty::font(TiskoveFonty::Styl styl) {
    if (!fontyNastavene_)
        nastavitFonty();

    return fonty_[styl];
}

void TiskoveFonty::nastavitFonty() {
    fonty_.insert(Normalni, QFont("Arial", 14));
    fonty_.insert(Popisek, QFont("Arial", 8));
    fonty_.insert(Jmeno, QFont("Arial", 26));

    fonty_[Normalni].setStyleHint(QFont::Helvetica);
    fonty_[Popisek].setStyleHint(QFont::Helvetica);
    fonty_[Popisek].setCapitalization(QFont::AllUppercase);
    fonty_[Jmeno].setStyleHint(QFont::Helvetica);
    fonty_[Jmeno].setWeight(QFont::Bold);
    fonty_[Jmeno].setCapitalization(QFont::AllUppercase);

    fontyNastavene_ = true;
}

bool TiskoveFonty::fontyNastavene_ = false;
QMap<TiskoveFonty::Styl, QFont> TiskoveFonty::fonty_ = QMap<TiskoveFonty::Styl, QFont>();
