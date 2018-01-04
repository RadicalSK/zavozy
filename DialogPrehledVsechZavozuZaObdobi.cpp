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
}

void DialogPrehledVsechZavozuZaObdobi::otevrit(QWidget *rodic) {
    DialogPrehledVsechZavozuZaObdobi *dialog = new DialogPrehledVsechZavozuZaObdobi(rodic);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->showMaximized();
    dialog->exec();
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
    QString zacatek = "Závozy ";
    if (volbaPoPomuckach_->isChecked()) {
        zacatek += "(položkovitě) ";
    }

    return QString(zacatek + "za období " +
            datumOd_.toString(Qt::ISODate) + " - " + datumDo_.toString(Qt::ISODate) + ".csv");
}
