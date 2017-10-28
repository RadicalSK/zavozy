#include "AktualizatorCiselniku.h"

AktualizatorCiselniku::AktualizatorCiselniku(QWidget *parent)
    : QObject(parent) {

}

bool AktualizatorCiselniku::aktualizovatCiselnik(QString souborCiselniku, QString uzivatel) {
    QFile soubor(souborCiselniku);

    QFileInfo souborInfo(soubor);
    QString kodVerze = souborInfo.baseName();
    if (!overitAktualnostVerze(kodVerze)) {
        OknoZprav::upozorneni(QString("Operaci nelze provést."),
                              QString("Už je nahraná stejná nebo novější verze číselníku: kód verze "
                                      + kodAktualniVerze_ + ", nahraná dne "
                                      + datumPosledniAktualizace_.toString("d. M. yyyy") + " uživatelem: "
                                      + naposledAktualizoval_));
        return false;
    }

    if (!soubor.open(QIODevice::ReadOnly | QIODevice::Text)) {
        OknoZprav::upozorneni(QString("Chyba při čtení souboru " + Text::kurziva(souborCiselniku) + "."),
                              QString("Soubor s číselníkem se nepodařilo otevřít. Číselník nebyl aktualizován."));
        return false;
    }

    if (!OknoZprav::potvrzeni(OknoZprav::Otazka,
                              "Aktualizace číselníku může trvat několik minut. Chcete pokračovat?")) {
        return false;
    }

    emit aktualizaceSpustena();

    int pribliznyPocetRadku = soubor.size() / 195;
    int pocitadloRadku = 0;
    int pridanychPolozek = 0;

    // postupne precte radky ze souboru, vytvori z nich SQL dotaz a vlozi nove zaznamy do tabulky
    QTextStream vstup(&soubor);
    vstup.setCodec("IBM 850");

    while (!vstup.atEnd()) {
        QStringList nticeHodnot;
        QStringList seznamNticHodnot;

        // pripravi seznam n-tic k ulozeni do databaze ve formatu "kodVzp_1, nazev_1, doplnek_1, kodVzp_2, nazev_2..."
        // bud z predepsaneho poctu radku, nebo ze zbytku radku do konce souboru
        for (int i = 0; i < pocetNticNaDotaz_; i++) {
            if (ziskatNticiHodnotZRadku(vstup.readLine(), nticeHodnot)) {
                seznamNticHodnot << nticeHodnot;
                if (vstup.atEnd())
                    break;
            }
            else {
                OknoZprav::upozorneni(QString("Chyba při čtení souboru " + Text::kurziva(souborCiselniku) + "."),
                                      QString("Při čtení řádků " + QString::number(pocitadloRadku) + " až "
                                              + QString::number(pocitadloRadku+pocetNticNaDotaz_)
                                              + " došlo k chybě. Soubor má špatný formát nebo je poškozen. "
                                                "Číselník nebyl kompletně aktualizován."));
                return false;
            }
        }

        if (Databaze::ulozitPomuckyDoCiselniku(seznamNticHodnot)) {
            pridanychPolozek += Databaze::pocetPolozekZmenenychPoslednimDotazem();
        }
        else {
            OknoZprav::chybaPriZapisuDoDb(QString("Položky od řádku " + QString::number(pocitadloRadku)
                                                  + " do konce souboru se nepodařilo uložit do databáze. "
                                                    "Číselník nebyl kompletně aktualizován."));
            return false;
        }

        pocitadloRadku += pocetNticNaDotaz_;
        emit zatimZpracovano((pocitadloRadku*100)/pribliznyPocetRadku);
    }

    emit zatimZpracovano(100);

    if (!Databaze::ulozitAktualniVerziCiselniku(kodVerze, uzivatel))
        OknoZprav::chybaPriZapisuDoDb("Kód aktuální verze číselníku nebyl uložen.");

    OknoZprav::informace(QString("Číselník byl úspěšně aktualizován na verzi " + kodVerze + ". Bylo přidáno "
                                 + QString::number(pridanychPolozek) + " nových položek."));

    emit aktualizaceDokoncena();
    return true;
}

bool AktualizatorCiselniku::overitAktualnostVerze(QString nazevSouboru) {
    if (Databaze::verzeCiselniku(kodAktualniVerze_, datumPosledniAktualizace_, naposledAktualizoval_)) {
        return (nazevSouboru > kodAktualniVerze_);
    }

    return true;
}

bool AktualizatorCiselniku::ziskatNticiHodnotZRadku(const QString radek, QStringList &ntice) {
    QRegularExpression regex("^\"(\\d+)\",\"([^\"]+)\",\"([^\"]*)\".*$");
    QRegularExpressionMatch shoda = regex.match(retezecZIbm850NaIbm852(radek));

    ntice.clear();

    if (shoda.hasMatch()) {
        ntice << shoda.captured(1) << shoda.captured(2) << shoda.captured(3);
        return true;
    }

    return false;
}

QString AktualizatorCiselniku::retezecZIbm850NaIbm852(QString retezec) {
    QString vysledek;
    for (int i = 0; i < retezec.size(); i++) {
        vysledek += znakZIbm850NaIbm852(retezec.at(i));
    }
    return vysledek;
}

QChar AktualizatorCiselniku::znakZIbm850NaIbm852(QChar znak) {
    switch (znak.unicode()) {
        case 0x00E0: return QChar(0x016F); break;
        case 0x00E5: return QChar(0x0107); break;
        case 0x00EA: return QChar(0x0142); break;
        case 0x00E8: return QChar(0x0150); break;
        case 0x00EF: return QChar(0x0151); break;
        case 0x00EC: return QChar(0x0179); break;
        case 0x00C5: return QChar(0x0106); break;
        case 0x00E6: return QChar(0x0139); break;
        case 0x00C6: return QChar(0x013A); break;
        case 0x00F2: return QChar(0x013D); break;
        case 0x00FB: return QChar(0x013E); break;
        case 0x00F9: return QChar(0x015A); break;
        case 0x00FF: return QChar(0x015B); break;
        case 0x00F8: return QChar(0x0164); break;
        case 0x00A3: return QChar(0x0165); break;
        case 0x00D8: return QChar(0x0141); break;
        case 0x0192: return QChar(0x010D); break;
        case 0x00F1: return QChar(0x0104); break;
        case 0x00D1: return QChar(0x0105); break;
        case 0x00AA: return QChar(0x017D); break;
        case 0x00BA: return QChar(0x017E); break;
        case 0x00BF: return QChar(0x0118); break;
        case 0x00AE: return QChar(0x0119); break;
        case 0x00BD: return QChar(0x017A); break;
        case 0x00BC: return QChar(0x010C); break;
        case 0x00A1: return QChar(0x015F); break;
        case 0x00C0: return QChar(0x011A); break;
        case 0x00A9: return QChar(0x015E); break;
        case 0x00A2: return QChar(0x017B); break;
        case 0x00A5: return QChar(0x017C); break;
        case 0x00E3: return QChar(0x0102); break;
        case 0x00C3: return QChar(0x0103); break;
        case 0x00F0: return QChar(0x0111); break;
        case 0x00D0: return QChar(0x0110); break;
        case 0x00CA: return QChar(0x010E); break;
        case 0x00C8: return QChar(0x010F); break;
        case 0x0131: return QChar(0x0147); break;
        case 0x00CF: return QChar(0x011B); break;
        case 0x00A6: return QChar(0x0162); break;
        case 0x00CC: return QChar(0x016E); break;
        case 0x00D2: return QChar(0x0143); break;
        case 0x00F5: return QChar(0x0144); break;
        case 0x00D5: return QChar(0x0148); break;
        case 0x00B5: return QChar(0x0160); break;
        case 0x00FE: return QChar(0x0161); break;
        case 0x00DE: return QChar(0x0154); break;
        case 0x00DB: return QChar(0x0155); break;
        case 0x00D9: return QChar(0x0170); break;
        case 0x00AF: return QChar(0x0163); break;
        case 0x00B1: return QChar(0x02DD); break;
        case 0x2017: return QChar(0x02DB); break;
        case 0x00BE: return QChar(0x02C7); break;
        case 0x00B6: return QChar(0x02D8); break;
        case 0x00B7: return QChar(0x02D9); break;
        case 0x00B9: return QChar(0x0171); break;
        case 0x00B3: return QChar(0x0158); break;
        case 0x00B2: return QChar(0x0159); break;
        default: return znak; break;
    }
}
