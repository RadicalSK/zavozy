#include "ValidatorVstupu.h"

ValidatorVstupu::ValidatorVstupu(QLabel *popisekUpozorneni)
    : popisekUpozorneni_(popisekUpozorneni)
{}

bool ValidatorVstupu::jeVyplneno(QLineEdit *policko, bool sUpozornenim) {
    if (policko->text().isEmpty()) {
        if (sUpozornenim)
            zobrazitUpozorneni(policko, true, "musí být vyplněno");
        return false;
    }
    else
        return true;
}

bool ValidatorVstupu::obsahujeJmenoNeboJmenoARok(QLineEdit *policko, bool sUpozornenim) {
    QRegularExpression jmenoNeboJmenoARok("^\\w+( \\d{4})?$");
    QRegularExpressionMatch shoda = jmenoNeboJmenoARok.match(policko->text());
    
    if (shoda.hasMatch()) {
        smazatUpozorneni(policko);
        return true;
    }
    else {
        if (sUpozornenim)
            zobrazitUpozorneni(policko, true, "musí obsahovat jméno, popř. následované mezerou a rokem narození");
        return false;
    }
}

bool ValidatorVstupu::obsahujeMinPocetSlov(QLineEdit *policko, int minPocetSlov, bool sUpozornenim) {
    QRegularExpression pocetSlovRegExp("\\b(\\w+)\\b");

    if (policko->text().count(pocetSlovRegExp) >= minPocetSlov) {
        smazatUpozorneni(policko);
        return true;
    }
    else {
        if (sUpozornenim)
            zobrazitUpozorneni(policko, true,
                               QString("musí obsahovat minimálně " + QString::number(minPocetSlov) + " slova"));
        return false;
    }
}

bool ValidatorVstupu::obsahujeTelCislo(QLineEdit *policko, bool sUpozornenim) {
    QRegularExpression telefonRegExp("^ *(\\d *){9}$");
    QRegularExpressionMatch shoda = telefonRegExp.match(policko->text());

    if (shoda.hasMatch()) {
        smazatUpozorneni(policko);
        return true;
    }
    else {
        if (sUpozornenim)
            zobrazitUpozorneni(policko, true, "musí obsahovat právě 9 číslic");
        return false;
    }
}

bool ValidatorVstupu::obsahujeTelCisloNeboJePrazdne(QLineEdit *policko, bool sUpozornenim) {
    if (policko->text().isEmpty()) {
        smazatUpozorneni(policko);
        return true;
    }
    else {
        if (obsahujeTelCislo(policko, false)) {
            return true;
        }
        else {
            if (sUpozornenim)
                zobrazitUpozorneni(policko, true, "musí být prázdné nebo obsahovat právě 9 číslic");
            return false;
        }
    }
}

bool ValidatorVstupu::maPredepsanouDelku(QLineEdit *policko, int predepsanaDelka, bool sUpozornenim) {
    if (policko->text().size() == predepsanaDelka) {
        smazatUpozorneni(policko);
        return true;
    }
    else {
        if (sUpozornenim)
            zobrazitUpozorneni(policko, true, QString("musí obsahovat právě "
                                                      + QString::number(predepsanaDelka) + " znaků"));
        return false;
    }
}

bool ValidatorVstupu::maPredepsanouDelkuNeboJePrazdne(QLineEdit *policko, int predepsanaDelka, bool sUpozornenim) {
    if (policko->text().isEmpty()) {
        smazatUpozorneni(policko);
        return true;
    }
    else {
        if (maPredepsanouDelku(policko, predepsanaDelka, false)) {
            return true;
        }
        else {
            if (sUpozornenim)
                zobrazitUpozorneni(policko, true, QString("musí být prázdné nebo obsahovat právě "
                                                          + QString::number(predepsanaDelka) + " znaků"));
            return false;
        }
    }
}

bool ValidatorVstupu::dosahujeMinDelku(QLineEdit *policko, int minDelka, bool sUpozornenim) {
    if (policko->text().size() >= minDelka) {
        smazatUpozorneni(policko);
        return true;
    }
    else {
        if (sUpozornenim)
            zobrazitUpozorneni(policko, true, QString("musí obsahovat minimálně " + QString::number(minDelka) + " znaků"));
        return false;
    }
}

bool ValidatorVstupu::dosahujeMinDelkuNeboJePrazdne(QLineEdit *policko, int minDelka, bool sUpozornenim) {
    if (policko->text().isEmpty()) {
        smazatUpozorneni(policko);
        return true;
    }
    else {
        if (dosahujeMinDelku(policko, minDelka, false)) {
            return true;
        }
        else {
            if (sUpozornenim)
                zobrazitUpozorneni(policko, true, QString("musí být prázdné nebo obsahovat minimálně "
                                                          + QString::number(minDelka) + " znaků"));
            return false;
        }
    }
}

bool ValidatorVstupu::shodujiSeHesla(QLineEdit *vstupHeslo1, QLineEdit *vstupHeslo2, bool sUpozornenim) {
    if (vstupHeslo1->text() == vstupHeslo2->text()) {
        zrusitZvyrazneniPolicka(vstupHeslo1);
        zrusitZvyrazneniPolicka(vstupHeslo2);
        smazatUpozorneni();
        return true;
    }
    else {
        zvyraznitPolicko(vstupHeslo1);
        zvyraznitPolicko(vstupHeslo2);
        if (sUpozornenim)
            zobrazitUpozorneni("Hesla se neshodují");
        return false;
    }
}

bool ValidatorVstupu::obsahujePsc(QLineEdit *policko, bool sUpozornenim) {
    QRegularExpression pscRegExp("^\\d{5}($|\\s)");
    QRegularExpressionMatch shoda = pscRegExp.match(policko->text());

    if (shoda.hasMatch()) {
        smazatUpozorneni(policko);
        return true;
    }
    else {
        if (sUpozornenim)
            zobrazitUpozorneni(policko, true, "musí začínat pětimístným PSČ");
        return false;
    }
}

void ValidatorVstupu::zvyraznitPolicko(QLineEdit *policko) {
    policko->setStyleSheet("background-color: salmon");
}

void ValidatorVstupu::zrusitZvyrazneniPolicka(QLineEdit *policko) {
    policko->setStyleSheet("");
}

void ValidatorVstupu::zobrazitUpozorneni(QString textUpozorneni) {
    popisekUpozorneni_->setText(textUpozorneni);
}

void ValidatorVstupu::zobrazitUpozorneni(QLineEdit *policko, bool zacitNazvemPolicka, QString textUpozorneni) {
    zvyraznitPolicko(policko);
    if (zacitNazvemPolicka)
        popisekUpozorneni_->setText(QString("Políčko " + Text::kurziva(policko->toolTip()) + " " + textUpozorneni));
    else
        popisekUpozorneni_->setText(textUpozorneni);
}

void ValidatorVstupu::zobrazitUpozorneni(QList<QLineEdit *> policka, QString textUpozorneni) {
    foreach (QLineEdit *policko, policka) {
        if (policko->isEnabled())
            zvyraznitPolicko(policko);
    }
    popisekUpozorneni_->setText(textUpozorneni);
}

void ValidatorVstupu::smazatUpozorneni() {
    popisekUpozorneni_->clear();
}

void ValidatorVstupu::smazatUpozorneni(QLineEdit *aktivniPolicko) {
    zrusitZvyrazneniPolicka(aktivniPolicko);
    popisekUpozorneni_->clear();
}

void ValidatorVstupu::nastavitFiltrUdalosti(QLineEdit *validovanyVstup) {
    validovanyVstup->installEventFilter(this);
}

void ValidatorVstupu::nastavitFiltrUdalosti(QList<QLineEdit*> validovaneVstupy) {
    foreach (QLineEdit *policko, validovaneVstupy) {
        nastavitFiltrUdalosti(policko);
    }
}

bool ValidatorVstupu::eventFilter(QObject *objekt, QEvent *udalost) {
    QLineEdit *policko = qobject_cast<QLineEdit *>(objekt);

    if (!policko)
        return false;

    // pri oznaceni policka smaze pripadne cervene pozadi
    if (udalost->type() == QEvent::FocusIn) {
        policko->setStyleSheet("");
    }
    // pri zmacknuti klavesy v policku vymaze popisek s upozornenim
    else if (udalost->type() == QEvent::KeyPress) {
        smazatUpozorneni();
    }

    return QObject::eventFilter(objekt, udalost);
}
