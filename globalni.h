#ifndef GLOBALNI_H
#define GLOBALNI_H

#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QMessageBox>
#include <QRegularExpression>

const QString cisloVerze = "v1.2";

namespace Soubory {
    const QStringList adresare({"data", "export", "temp", "zalohy"});
    const QString databaze = "data/zavozy.dat";
    const QString psc = "data/psc.txt";
    const QString zalohaDbVzor = "zalohy/databaze-*.bak";
    const QString nevytistene = "temp/nevytistene.json";
    const QString vytisteneVzor = "temp/vytistene-*.json";
    const QString tisk = "temp/tisk-pruvodky.pdf";
    const QString ciselnikWeb = "http://szpcr.cz/zdravotnicke_prostredky";
    const QString prirucka = "data/prirucka.html";
    const QString zmeny = "data/zmeny.html";
    const int pocetAktivnichZaloh = 5;

    // operace pri startu aplikace
    void vytvoritAdresare();
    void odstranitNeaktualniSouborVytistene();
    void odstranitNadbytecneZalohyDb();

    // operace se soubory tisku
    bool existujiNevytistene();
    void odstranitNevytistene();
    QString souborDnesVytistene();
    bool existujiDnesVytistene();
    void odstranitTiskovySoubor();

    // operace se soubory databaze
    bool zalohovatDatabazi(QString *verzeDb = nullptr);
    bool obnovitZalohu(QString souborZaloha, QString &verzeDb);
}

namespace OknoZprav {
    enum TypPotvrzeni {
        Otazka = QMessageBox::Question,
        Varovani = QMessageBox::Warning
    };

    void informace(QString text);
    void upozorneni(QString zakladniText, QString doplnujiciText);
    void kritickaChyba(QString zakladniText, QString doplnujiciText);
    void chybaPriZapisuDoDb(QString vysvetlujiciText);

    bool potvrzeni(TypPotvrzeni typ, QString zakladniText, QString doplnujiciText);
    bool potvrzeni(TypPotvrzeni typ, QString text);
}

namespace Text {
    QString cervene(const QString retezec);
    QString kurziva(const QString retezec);
    QString kurziva(const QString retezec1, const QString retezec2);
}

namespace Heslo {
    const int minDelka = 5;

    QString sha256Hash(QString heslo);
}

#endif // GLOBALNI_H
