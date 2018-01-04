#ifndef VALIDATORVSTUPU_H
#define VALIDATORVSTUPU_H

#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QRegularExpressionMatch>
#include <QString>
#include "globalni.h"

class ValidatorVstupu : public QObject
{
public:
    ValidatorVstupu(QLabel *popisekUpozorneni);

    bool jeVyplneno(QLineEdit *policko, bool sUpozornenim = true);
    bool obsahujeJmenoNeboJmenoARok(QLineEdit *policko, bool sUpozornenim = true);
    bool obsahujeMinPocetSlov(QLineEdit *policko, int minPocetSlov, bool sUpozornenim = true);
    bool obsahujeTelCislo(QLineEdit *policko, bool sUpozornenim = true);
    bool obsahujeTelCisloNeboJePrazdne(QLineEdit *policko, bool sUpozornenim = true);
    bool maPredepsanouDelku(QLineEdit *policko, int predepsanaDelka, bool sUpozornenim = true);
    bool maPredepsanouDelkuNeboJePrazdne(QLineEdit *policko, int predepsanaDelka, bool sUpozornenim = true);
    bool dosahujeMinDelku(QLineEdit *policko, int minDelka, bool sUpozornenim = true);
    bool dosahujeMinDelkuNeboJePrazdne(QLineEdit *policko, int minDelka, bool sUpozornenim = true);
    bool shodujiSeHesla(QLineEdit *vstupHeslo1, QLineEdit *vstupHeslo2, bool sUpozornenim = true);
    bool obsahujePsc(QLineEdit *policko, bool sUpozornenim = true);

    void zvyraznitPolicko(QLineEdit *policko);
    void zrusitZvyrazneniPolicka(QLineEdit *policko);
    void zobrazitUpozorneni(QString textUpozorneni);
    void zobrazitUpozorneni(QLineEdit *policko, bool zacitNazvemPolicka, QString textUpozorneni);
    void zobrazitUpozorneni(QList<QLineEdit*> policka, QString textUpozorneni);
    void smazatUpozorneni();
    void smazatUpozorneni(QLineEdit *aktivniPolicko);

    void nastavitFiltrUdalosti(QLineEdit *validovanyVstup);
    void nastavitFiltrUdalosti(QList<QLineEdit*> validovaneVstupy);

protected:
    bool eventFilter(QObject *objekt, QEvent *udalost);

private:
    QLabel *popisekUpozorneni_;
};

#endif // VALIDATORVSTUPU_H
