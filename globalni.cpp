#include "globalni.h"

// SOUBORY
void Soubory::vytvoritAdresare() {
    QDir adresarAplikace(QDir::current());

    foreach (auto adresar, adresare) {
        adresarAplikace.mkpath(adresar);
    }
}

void Soubory::odstranitNeaktualniSouborVytistene() {
    QString nazevDnesVytistene = souborDnesVytistene().section("/", -1);

    QDirIterator it("./temp", QStringList(vytisteneVzor.section("/", -1)));
    while (it.hasNext()) {
        it.next();
        if (it.fileName() != nazevDnesVytistene) {
            QFile::remove(it.filePath());
        }
    }
}

void Soubory::odstranitNadbytecneZalohyDb() {
    QStringList souboryZalohyDb;

    QDirIterator it("./zalohy", QStringList(zalohaDbVzor.section("/", -1)));
    while (it.hasNext()) {
        souboryZalohyDb << it.next();
    }

    souboryZalohyDb.sort();
    for (int i = 0; i < souboryZalohyDb.size() - pocetAktivnichZaloh; i++) {
        QFile::remove(souboryZalohyDb.at(i));
    }
}

bool Soubory::existujiNevytistene() {
    return QFile::exists(nevytistene);
}

void Soubory::odstranitNevytistene() {
    QFile::remove(nevytistene);
}

QString Soubory::souborDnesVytistene() {
    return QString(vytisteneVzor).replace("*", QDate::currentDate().toString(Qt::ISODate));
}

bool Soubory::existujiDnesVytistene() {
    return QFile::exists(souborDnesVytistene());
}

void Soubory::odstranitTiskovySoubor() {
    QFile::remove(tisk);
}

bool Soubory::zalohovatDatabazi(QString *verzeDb) {
    QString verze = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QString souborZaloha = QString(zalohaDbVzor).replace("*", verze);
    if (verzeDb)
        *verzeDb = verze;
    return QFile::copy(databaze, souborZaloha);
}

bool Soubory::obnovitZalohu(QString souborZaloha, QString &verzeDb) {
    QRegularExpression regex(".*(\\d{14}).*");
    QRegularExpressionMatch shoda = regex.match(souborZaloha);

    if (shoda.hasMatch()) {
        verzeDb = shoda.captured(1);
    }
    else {
        return false;
    }

    QString souborDocasnaZaloha = QString(databaze + ".tmp");

    QFile::rename(databaze, souborDocasnaZaloha);

    if (QFile::copy(souborZaloha, databaze)) {
        QFile::remove(souborDocasnaZaloha);
        return true;
    }
    else {
        QFile::rename(souborDocasnaZaloha, databaze);
        return false;
    }
}

// OKNO ZPRAV
void OknoZprav::informace(QString text) {
    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void OknoZprav::upozorneni(QString zakladniText, QString doplnujiciText) {
    QMessageBox msgBox;
    msgBox.setText(zakladniText);
    msgBox.setInformativeText(doplnujiciText);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void OknoZprav::kritickaChyba(QString zakladniText, QString doplnujiciText) {
    QMessageBox msgBox;
    msgBox.setText(zakladniText);
    msgBox.setInformativeText(doplnujiciText);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void OknoZprav::chybaPriZapisuDoDb(QString vysvetlujiciText) {
    upozorneni("Chyba při zápisu do databáze.", vysvetlujiciText);
}

bool OknoZprav::potvrzeni(TypPotvrzeni typ, QString zakladniText, QString doplnujiciText) {
    QMessageBox dialog;

    switch (typ) {
        case Otazka: dialog.setIcon(QMessageBox::Question);
            break;
        case Varovani: dialog.setIcon(QMessageBox::Warning);
            break;
    }

    dialog.setText(zakladniText);
    dialog.setInformativeText(doplnujiciText);
    dialog.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    dialog.setDefaultButton(QMessageBox::Cancel);
    dialog.setButtonText(QMessageBox::Cancel, "Zrušit");
    return (dialog.exec() == QMessageBox::Ok);
}

bool OknoZprav::potvrzeni(TypPotvrzeni typ, QString text) {
    return potvrzeni(typ, text, "");
}

// HESLO
QString Heslo::sha256Hash(QString heslo) {
    return QString(QCryptographicHash::hash(heslo.toUtf8(), QCryptographicHash::Sha256).toHex());
}

// TEXT
QString Text::cervene(const QString retezec) {
    return QString("<span style=\"color: red\">" + retezec + "</span>");
}

QString Text::kurziva(const QString retezec) {
    return QString("<i>" + retezec + "</i>");
}

QString Text::kurziva(const QString retezec1, const QString retezec2) {
    return QString("<i>" + retezec1 + " " + retezec2 + "</i>");
}

QString Text::odstranitBileZnaky(const QString retezec) {
    return QString(retezec).replace(QRegularExpression("\\s"), "");
}

QDate Datum::denNejblizsihoZavozu() {
    QDate dnes = QDate::currentDate();

    switch (dnes.dayOfWeek()) {
        case 1: return dnes.addDays(2);
            break;
        case 2: return dnes.addDays(1);
            break;
        case 3: return dnes.addDays(2);
            break;
        case 4: return dnes.addDays(1);
            break;
        case 5: return dnes.addDays(5);
            break;
        case 6: return dnes.addDays(4);
            break;
        case 7: return dnes.addDays(3);
            break;
        default: return dnes;
    }
}

QDate Datum::prvniDenPredchozihoMesice() {
    QDate dnes = QDate::currentDate();

    int rok, mesic;

    if (dnes.month() > 1) {
        rok = dnes.year();
        mesic = dnes.month() - 1;
    }
    else {
        rok = dnes.year() - 1;
        mesic = 12;
    }

    return QDate(rok, mesic, 1);
}

QDate Datum::posledniDenPredchozihoMesice() {
    QDate dnes = QDate::currentDate();

    int rok, mesic;

    if (dnes.month() > 1) {
        rok = dnes.year();
        mesic = dnes.month() - 1;
    }
    else {
        rok = dnes.year() - 1;
        mesic = 12;
    }

    QDate datum = QDate(rok, mesic, 1);
    datum.setDate(rok, mesic, datum.daysInMonth());
    return datum;
}

QDate Datum::prvniDenPredchozihoKvartalu() {
    QDate dnes = QDate::currentDate();

    int rok, mesic;

    switch (dnes.month()) {
        case 1:
        case 2:
        case 3:
            rok = dnes.year() - 1;
            mesic = 10;
            break;
        case 4:
        case 5:
        case 6:
            rok = dnes.year();
            mesic = 1;
            break;
        case 7:
        case 8:
        case 9:
            rok = dnes.year();
            mesic = 4;
            break;
        case 10:
        case 11:
        case 12:
        default:
            rok = dnes.year();
            mesic = 7;
    }

    return QDate(rok, mesic, 1);
}

QDate Datum::posledniDenPredchozihoKvartalu() {
    QDate dnes = QDate::currentDate();

    int rok, mesic;

    switch (dnes.month()) {
        case 1:
        case 2:
        case 3:
            rok = dnes.year() - 1;
            mesic = 12;
            break;
        case 4:
        case 5:
        case 6:
            rok = dnes.year();
            mesic = 3;
            break;
        case 7:
        case 8:
        case 9:
            rok = dnes.year();
            mesic = 6;
            break;
        case 10:
        case 11:
        case 12:
        default:
            rok = dnes.year();
            mesic = 9;
    }

    QDate datum = QDate(rok, mesic, 1);
    datum.setDate(rok, mesic, datum.daysInMonth());
    return datum;
}

QDate Datum::prvniDenPrechodzihoRoku() {
    QDate dnes = QDate::currentDate();

    return QDate(dnes.year() - 1, 1, 1);
}

QDate Datum::posledniDenPredchozihoRoku() {
    QDate dnes = QDate::currentDate();

    return QDate(dnes.year() - 1, 12, 31);
}
