#include "DialogPrehledZavozuVychystanychUzivatelem.h"
#include "ui_AbstraktniDialogPrehled.h"

DialogPrehledZavozuVychystanychUzivatelem::DialogPrehledZavozuVychystanychUzivatelem(QWidget *rodic)
    : AbstraktniDialogPrehled(rodic),
      comboBoxUzivatel_(new QComboBox), popisekUzivatel_(new QLabel)
{
    // prizbusobi texty
    this->setWindowTitle("Přehled závozů vychystaných uživatelem");
    ui->sekceVyhledavaciKriterium->setTitle("Uživatel:");

    // prida potrebne widgety do rozvrzeni
    QLayout *rozvrzeniSekce = ui->sekceVyhledavaciKriterium->layout();
    rozvrzeniSekce->addWidget(comboBoxUzivatel_);
    rozvrzeniSekce->addWidget(popisekUzivatel_);

    // nastavi widgety
    comboBoxUzivatel_->setMaximumWidth(200);
    comboBoxUzivatel_->addItem("(zvolte uživatele)");
    foreach (QString uzivatel, Databaze::seznamUzivatelu(false)) {
        comboBoxUzivatel_->addItem(uzivatel);
    }
    comboBoxUzivatel_->setFocus();

    popisekUzivatel_->setStyleSheet("font-weight: bold");

    // navaze udalosti
    connect(comboBoxUzivatel_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DialogPrehledZavozuVychystanychUzivatelem::zvolitUzivatele);
}

void DialogPrehledZavozuVychystanychUzivatelem::otevrit(QWidget *rodic) {
    DialogPrehledZavozuVychystanychUzivatelem *dialog = new DialogPrehledZavozuVychystanychUzivatelem(rodic);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->showMaximized();
    dialog->exec();
}

void DialogPrehledZavozuVychystanychUzivatelem::zvolitUzivatele(int cisloZaznamu) {
    bool uzivatelZvolen = (cisloZaznamu > 0);
    if (uzivatelZvolen) {
        QString uzivJmeno = comboBoxUzivatel_->currentText().section(" ", 0, 0);
        QString celeJmeno;
        bool admin;

        Databaze::nacistUdajeUzivatele(uzivJmeno, celeJmeno, admin);

        popisekUzivatel_->setText(celeJmeno);
    }
    else {
        popisekUzivatel_->clear();
    }

    povolitVyhledavaciTlacitko(uzivatelZvolen);
    validator_->smazatUpozorneni();
    zpristupnitTabulku(false);
}

bool DialogPrehledZavozuVychystanychUzivatelem::vyhledatZaznamy() {
    return Databaze::prehledZavozuUzivatele(comboBoxUzivatel_->currentText().section(" ", 0, 0),
                                            datumOd_, datumDo_, queryModel_);
}

QString DialogPrehledZavozuVychystanychUzivatelem::navrhnoutNazevCsvSouboru() {
    return QString(comboBoxUzivatel_->currentText().section(" ", 0, 0) + " - závozy "
                   + datumOd_.toString(Qt::ISODate) + " - " + datumDo_.toString(Qt::ISODate) + ".csv");
}
