#include "WidgetSubsekcePomuckySeznam.h"

WidgetSubsekcePomuckySeznam::WidgetSubsekcePomuckySeznam(QWidget *rodic)
    : QWidget(rodic), hlavniOkno_(rodic),
      vertikalniVycpavka_(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Expanding))
{
    mrizka_ = new QGridLayout(this);
    mrizka_->setColumnMinimumWidth(0, 60);

    this->setLayout(mrizka_);

    connect(this, SIGNAL(signalRadekOdstranen()),
            hlavniOkno_, SLOT(aktivovatVstupNovaPolozka()));
}

bool WidgetSubsekcePomuckySeznam::neniDuplicita(QString kodVzp) {
    return !pouziteKodyVzp_.values().contains(kodVzp.toInt());
}

WidgetVstupniPolePoctu *WidgetSubsekcePomuckySeznam::vstupniPolePoctuPolozky(QString kodVzp) {
    return radkyMrizky_.value(pouziteKodyVzp_.key(kodVzp.toInt())).vstupniPolePoctu;
}

WidgetVstupniPolePoctu *WidgetSubsekcePomuckySeznam::aktualniVstupniPolePoctu() {
    if (radkyMrizky_.isEmpty())
        return nullptr;
    else
        return radkyMrizky_.last().vstupniPolePoctu;
}

QList<PolozkaZavozu> WidgetSubsekcePomuckySeznam::zvolenePomucky() {
    QList<PolozkaZavozu> zvolenePomucky;
    QList<int> pouzitaCislaPolozek = pouziteKodyVzp_.keys();

    int kodVzp, pocet;
    QString nazev, doplnek;

    for (int i = 0; i < pouzitaCislaPolozek.size(); i++) {
        int cisloRadku = pouzitaCislaPolozek.at(i);
        pocet = radkyMrizky_[cisloRadku].vstupniPolePoctu->value();
        kodVzp = radkyMrizky_[cisloRadku].popisekKoduVzp->text().toInt();
        nazev = radkyMrizky_[cisloRadku].popisekNazvu->text().section('\n', 0, 0);
        doplnek = radkyMrizky_[cisloRadku].popisekNazvu->text().section('\n', 1, 1);

        if (pocet) {
            zvolenePomucky.append(PolozkaZavozu(pocet, kodVzp, nazev, doplnek));
        }
    }

    return zvolenePomucky;
}

void WidgetSubsekcePomuckySeznam::pridatRadek(QString kodVzp, QString nazev, QString doplnek, bool jeNova) {
    mrizka_->removeItem(vertikalniVycpavka_);

    int cRadku = mrizka_->rowCount();
    int cSloupce = 0;

    // vytvori radek grafickych prvku a vlozi do mapy radku
    RadekMrizky radek = radekMrizky(kodVzp, nazev, doplnek, jeNova);
    radkyMrizky_.insert(cisloVlozenePolozky_, radek);

    // prida graficke prvky radku do mrizky
    mrizka_->addWidget(radek.popisekKoduVzp, cRadku, cSloupce++);
    mrizka_->addWidget(radek.popisekNazvu, cRadku, cSloupce++);
    mrizka_->addWidget(radek.vstupniPolePoctu, cRadku, cSloupce++);
    mrizka_->addWidget(radek.popisekKs, cRadku, cSloupce++);
    mrizka_->addItem(radek.vycpavka, cRadku, cSloupce++);
    if (jeNova)
        mrizka_->addWidget(radek.tlacitkoOdstraneni, cRadku, cSloupce++);

    mrizka_->addItem(vertikalniVycpavka_, (cRadku+1), 0);

    // vytvori vazby udalosti
    connect(radek.vstupniPolePoctu, SIGNAL(signalHodnotaZmenena(bool)),
            hlavniOkno_, SLOT(nastavitNeulozeneZmeny(bool)));

    connect(radek.vstupniPolePoctu, SIGNAL(signalCtrlEnterStisknuto()),
            hlavniOkno_, SLOT(on_tlacitkoVytvoritPruvodku_clicked()));

    connect(radek.vstupniPolePoctu, &WidgetVstupniPolePoctu::signalCtrlDelStisknuto,
            this, &WidgetSubsekcePomuckySeznam::odstranitRadek);

    if (jeNova) {
        connect(radek.tlacitkoOdstraneni, &WidgetOdstranovaciTlacitko::signalOdstranitRadek,
                this, &WidgetSubsekcePomuckySeznam::odstranitRadek);
    }

    // vlozi kod VZP do pouzitych kodu
    pouziteKodyVzp_.insert(cisloVlozenePolozky_, kodVzp.toInt());

    // zvetsi cislo nasledujici polozky
    cisloVlozenePolozky_++;
}

void WidgetSubsekcePomuckySeznam::odstranitRadek(int cisloVlozenePolozky) {    
    RadekMrizky radek = radkyMrizky_.value(cisloVlozenePolozky);

    // odstrani prvky z mrizky
    mrizka_->removeWidget(radek.popisekKoduVzp);
    mrizka_->removeWidget(radek.popisekNazvu);
    mrizka_->removeWidget(radek.vstupniPolePoctu);
    mrizka_->removeWidget(radek.popisekKs);
    mrizka_->removeItem(radek.vycpavka);
    mrizka_->removeWidget(radek.tlacitkoOdstraneni);

    // odstrani radek z mapy radku a vymaze jeho prvky z pameti
    smazatPrvkyRadku(radkyMrizky_.take(cisloVlozenePolozky));

    // odstrani kod VZP z pouzitych kodu
    pouziteKodyVzp_.remove(cisloVlozenePolozky);

    emit signalRadekOdstranen();
}

void WidgetSubsekcePomuckySeznam::odstranitPosledniRadek() {
    // smaze posledni radek, jenom pokud ma odstranovaci tlacitko
    if (radkyMrizky_.last().tlacitkoOdstraneni) {
        odstranitRadek(radkyMrizky_.lastKey());
    }
}

void WidgetSubsekcePomuckySeznam::vyprazdnit() {
    // vymaze vsechny prvky z mrizky
    foreach (QWidget *potomek, this->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly)) {
        delete potomek;
    }

    // obnovi stav mrizky na pocatecni hodnotu
    cisloVlozenePolozky_ = 0;
    pouziteKodyVzp_.clear();
    radkyMrizky_.clear();
}

WidgetSubsekcePomuckySeznam::RadekMrizky WidgetSubsekcePomuckySeznam::radekMrizky(QString kodVzp, QString nazev, QString doplnek, bool jeNova) {
    RadekMrizky radek;

    radek.popisekKoduVzp = new QLabel(QString("%1").arg(kodVzp, 7, '0'));
    radek.popisekKoduVzp->setStyleSheet("font-weight: bold");

    QString celyNazev = nazev;
    if (!doplnek.isEmpty())
        celyNazev += '\n' + doplnek;
    radek.popisekNazvu = new QLabel(celyNazev);

    radek.vstupniPolePoctu = new WidgetVstupniPolePoctu(cisloVlozenePolozky_, jeNova);

    radek.popisekKs = new QLabel("ks");

    radek.vycpavka = new QSpacerItem(10, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);

    if (jeNova)
        radek.tlacitkoOdstraneni = new WidgetOdstranovaciTlacitko(cisloVlozenePolozky_);

    return radek;
}

void WidgetSubsekcePomuckySeznam::smazatPrvkyRadku(WidgetSubsekcePomuckySeznam::RadekMrizky radek) {
    delete radek.popisekKoduVzp;
    delete radek.popisekNazvu;
    delete radek.vstupniPolePoctu;
    delete radek.popisekKs;
    delete radek.vycpavka;
    delete radek.tlacitkoOdstraneni;
}
