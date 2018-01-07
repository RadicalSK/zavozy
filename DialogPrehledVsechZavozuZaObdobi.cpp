#include "DialogPrehledVsechZavozuZaObdobi.h"
#include "ui_AbstraktniDialogPrehled.h"

DialogPrehledVsechZavozuZaObdobi::DialogPrehledVsechZavozuZaObdobi(QWidget *rodic)
    : AbstraktniDialogPrehled(rodic),
      stalyPopisekZaznamyZobrazit_(new QLabel("Záznamy zobrazit:")),
      volbaPoPacientech_(new QRadioButton("po pacientech")), volbaPoPomuckach_(new QRadioButton("po jednotlivých pomůckách"))
{
    // prizbusobi texty
    this->setWindowTitle("Přehled všech závozů za období");
    ui->sekceVyhledavaciKriterium->setTitle("Typ zobrazení:");

    // prida potrebne widgety do rozvrzeni
    QLayout *rozvrzeniSekce = ui->sekceVyhledavaciKriterium->layout();
    rozvrzeniSekce->addWidget(stalyPopisekZaznamyZobrazit_);
    rozvrzeniSekce->addWidget(volbaPoPacientech_);
    rozvrzeniSekce->addWidget(volbaPoPomuckach_);
    rozvrzeniSekce->addItem(new QSpacerItem(10, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));

    // nastavi widgety
    volbaPoPacientech_->setChecked(true);
    ui->comboBoxObdobi->setCurrentText("Den nejbližšího závozu");
    povolitVyhledavaciTlacitko(true);
    ui->tlacitkoVyhledat->setFocus();

    // navaze udalosti
    connect(volbaPoPacientech_, &QRadioButton::toggled,
            this, &DialogPrehledVsechZavozuZaObdobi::volbaZmenena);

    connect(volbaPoPomuckach_, &QRadioButton::toggled,
            this, &DialogPrehledVsechZavozuZaObdobi::volbaZmenena);
}

void DialogPrehledVsechZavozuZaObdobi::otevrit(QWidget *rodic) {
    DialogPrehledVsechZavozuZaObdobi *dialog = new DialogPrehledVsechZavozuZaObdobi(rodic);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->showMaximized();
    dialog->exec();
}

void DialogPrehledVsechZavozuZaObdobi::volbaZmenena(bool zaskrtnuto) {
    if (zaskrtnuto) {
        zpristupnitTabulku(false);
    }
}

bool DialogPrehledVsechZavozuZaObdobi::vyhledatZaznamy() {
    if (volbaPoPacientech_->isChecked()) {
        return Databaze::prehledZavozuPoPacientech(datumOd_, datumDo_, queryModel_);
    }
    else if (volbaPoPomuckach_->isChecked()) {
        return Databaze::prehledZavozu(datumOd_, datumDo_, queryModel_);
    }
    else {
        return false;
    }
}

QString DialogPrehledVsechZavozuZaObdobi::navrhnoutNazevCsvSouboru() {
    QString zacatek = "Závozy";
    if (volbaPoPomuckach_->isChecked()) {
        zacatek += " (položkovitě)";
    }

    return QString("%1 za období %2.csv").arg(zacatek).arg(AbstraktniDialogPrehled::casoveRozpetiProNazev());
}
