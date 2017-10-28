#include "TiskarnaPruvodek.h"

TiskarnaPruvodek::TiskarnaPruvodek(const QList<Zavoz> seznamPruvodek) {
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Portrait);
    printer.setFullPage(true);
    printer.setOutputFileName(Soubory::tisk);
    printer.setOutputFormat(QPrinter::PdfFormat);

    int sirkaStrany = printer.width();
    int vyskaStrany = printer.height();
    int okraj = 0.07 * sirkaStrany;
    int sirkaSekce = (sirkaStrany - 2 * okraj);
    int vyskaSekce = (vyskaStrany / 2) - (2 * okraj);

    horniPulka_ = QRect(okraj, okraj, sirkaSekce, vyskaSekce);
    dolniPulka_ = QRect(okraj, (vyskaStrany / 2) + okraj, sirkaSekce, vyskaSekce);

    QPainter painter(&printer);

    int cisloTistenehoZavozu;
    int cisloTistenePruvodky = 0;   // jeden zavoz muze zabirat vic pruvodek

    for (cisloTistenehoZavozu = 0; cisloTistenehoZavozu < seznamPruvodek.size(); cisloTistenehoZavozu++) {
        Zavoz zavoz = seznamPruvodek.at(cisloTistenehoZavozu);
        bool jePoznamka = !zavoz.poznamka().isEmpty();
        int pocetPolozekKTisku = zavoz.polozky().size();
        int pocetDodatecnychPruvodek = 0;   // pocet pruvodek nutnych pro vytisteni nadmerneho poctu polozek

        do {
            TiskoveRozvrzeniPruvodky rozvrzeni((cisloTistenePruvodky % 2) ? dolniPulka_ : horniPulka_, &painter, jePoznamka);

            // vytiskne oramovani
            QPen pen(QBrush(Qt::SolidPattern), 25);
            painter.setPen(pen);
            painter.drawRects(rozvrzeni.rameckyKTisku());

            // vytiskne popisky
            QStringList popisky = {"Jméno:", "Adresa:", "Telefon:", "Pomůcky:", "Poznámka:"};

            painter.setFont(TiskoveFonty::font(TiskoveFonty::Popisek));
            for (int i = 0; i < popisky.size(); i++) {
                painter.drawText(rozvrzeni.obdelnikyPopisku().at(i), Qt::AlignLeft, popisky.at(i));
            }

            // vytiskne jmeno, adresu, telefon a poznamku
            painter.setFont(TiskoveFonty::font(TiskoveFonty::Jmeno));
            painter.drawText(rozvrzeni.obdelnikObsahuJmeno(), Qt::AlignHCenter, zavoz.celeJmeno());

            painter.setFont(TiskoveFonty::font(TiskoveFonty::Normalni));
            painter.drawText(rozvrzeni.obdelnikObsahuAdresa(), Qt::AlignLeft, zavoz.adresaDvojradkove());
            painter.drawText(rozvrzeni.obdelnikObsahuTelefon(), Qt::AlignLeft, zavoz.telefony());
            if (jePoznamka)
                painter.drawText(rozvrzeni.obdelnikObsahuPoznamka(), Qt::AlignHCenter, zavoz.poznamka());

            // vytiskne radky pomucek
            QVector<QRect> obdelnikyPomucek = rozvrzeni.obdelnikyPomucek();
            int maxPolozek = rozvrzeni.maxPocetRadkuPomucek();
            int j;

            for (j = 0; j < pocetPolozekKTisku && j < maxPolozek; j++) {
                painter.drawText(obdelnikyPomucek.at(j), Qt::AlignLeft,
                                 rozvrzeni.zkratitTextRadkuPomucky(textRadkuPolozky(zavoz.polozky().at(j + maxPolozek * pocetDodatecnychPruvodek))));
            }

            pocetPolozekKTisku = pocetPolozekKTisku - j;
            pocetDodatecnychPruvodek++; // pokud nezustaly zadne polozky k tisku, nova hodnota nebude pouzita

            // otevrit novou stranku, pokud se posledni pruvodka tiskla na dolni pulku
            // a zaroven plati jeste existuji bud polozky k tisku z aktualniho zavozu, nebo nezpracovane zavozy
            if ((cisloTistenePruvodky % 2) && ((pocetPolozekKTisku > 0) || (cisloTistenehoZavozu < (seznamPruvodek.size() - 1)))) {
                printer.newPage();
            }
        } while (cisloTistenePruvodky++, pocetPolozekKTisku > 0); // c. pruvodky stoupa nezavisle od cisla tisteneho zavozu
    }
}

QString TiskarnaPruvodek::textRadkuPolozky(PolozkaZavozu polozka) {
    if (polozka.kodVzp())   // nenulovy kod = pomucka
        return QString(QString::number(polozka.pocet()) + "× "
                       + QString("%1").arg(polozka.kodVzp(), 7, 10, QChar('0')) + " "
                       + polozka.celyNazevJednoradkove());
    else    // nulovy kod = jina polozka nez pomucka
        return QString(QString::number(polozka.pocet()) + "× " + polozka.celyNazevJednoradkove());
}
