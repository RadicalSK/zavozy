#include "DialogPrehledZavozuProPacienta.h"
#include "ui_AbstraktniDialogPrehled.h"

DialogPrehledZavozuProPacienta::DialogPrehledZavozuProPacienta(QWidget *rodic)
    : AbstraktniDialogPrehled(rodic),
      stalyPopisekPrijmeni_(new QLabel("Příjmení:")), stalyPopisekJmeno_(new QLabel("Jméno:")),
      vstupPrijmeni_(new QLineEdit), vstupJmeno_(new QLineEdit),
      tlacitkoNajitVDatabazi_(new QPushButton("Najít v databázi")),
      tlacitkoJinyPacient_(new QPushButton("Jiný pacient"))
{
    // prizbusobi texty
    this->setWindowTitle("Přehled závozů pro pacienta");
    ui->sekceVyhledavaciKriterium->setTitle("Pacient:");

    // prida potrebne widgety do rozvrzeni
    QLayout *rozvrzeniSekce = ui->sekceVyhledavaciKriterium->layout();
    rozvrzeniSekce->addWidget(stalyPopisekPrijmeni_);
    rozvrzeniSekce->addWidget(vstupPrijmeni_);
    rozvrzeniSekce->addWidget(stalyPopisekJmeno_);
    rozvrzeniSekce->addWidget(vstupJmeno_);
    rozvrzeniSekce->addWidget(tlacitkoNajitVDatabazi_);
    rozvrzeniSekce->addItem(new QSpacerItem(10, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
    rozvrzeniSekce->addWidget(tlacitkoJinyPacient_);

    // nastavi widgety
    vstupPrijmeni_->setMaximumWidth(100);
    vstupPrijmeni_->setCompleter(naseptavac_->naseptavacPrijmeni());
    vstupPrijmeni_->setFocus();
    vstupJmeno_->setMaximumWidth(100);
    tlacitkoNajitVDatabazi_->setMinimumWidth(120);
    tlacitkoJinyPacient_->setMinimumWidth(120);
    tlacitkoJinyPacient_->setVisible(false);

    // navaze udalosti
    connect(vstupPrijmeni_, &QLineEdit::returnPressed,
            this, &DialogPrehledZavozuProPacienta::prepnoutNaVstupJmeno);

    connect(vstupJmeno_, &QLineEdit::returnPressed,
            tlacitkoNajitVDatabazi_, &QPushButton::click);

    connect(tlacitkoNajitVDatabazi_, &QPushButton::clicked,
            this, &DialogPrehledZavozuProPacienta::vyhledatPacientaVDatabazi);

    connect(tlacitkoJinyPacient_, &QPushButton::clicked,
            this, &DialogPrehledZavozuProPacienta::zrusitVolbuPacienta);
}

DialogPrehledZavozuProPacienta::DialogPrehledZavozuProPacienta(Pacient *pacient, QWidget *rodic)
    : DialogPrehledZavozuProPacienta(rodic) {
    vstupPrijmeni_->setText(pacient->prijmeni());
    vstupJmeno_->setText(pacient->jmeno());

    // vyhleda pacienta a znemozni vyhledani jineho
    tlacitkoNajitVDatabazi_->click();
    tlacitkoJinyPacient_->setVisible(false);

    // rovnou vyhleda a zobrazi zaznamy za celou dobu
    ui->comboBoxObdobi->setCurrentIndex(CelaDoba);
    zobrazitZaznamy();
}

void DialogPrehledZavozuProPacienta::otevrit(QWidget *rodic) {
    DialogPrehledZavozuProPacienta *dialog = new DialogPrehledZavozuProPacienta(rodic);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->showMaximized();
    dialog->exec();
}

void DialogPrehledZavozuProPacienta::otevritZavozyAktualnihoPacienta(Pacient *pacient, QWidget *rodic) {
    DialogPrehledZavozuProPacienta *dialog = new DialogPrehledZavozuProPacienta(pacient, rodic);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->showMaximized();
    dialog->exec();
}

void DialogPrehledZavozuProPacienta::prepnoutNaVstupJmeno() {
    if (!vstupPrijmeni_->text().isEmpty()) {
    vstupJmeno_->setCompleter(naseptavac_->naseptavacJmen(vstupPrijmeni_->text()));
    vstupJmeno_->setFocus();
    vstupJmeno_->selectAll();
    }
}

void DialogPrehledZavozuProPacienta::vyhledatPacientaVDatabazi() {
    if (validator_->jeVyplneno(vstupPrijmeni_, false) && validator_->jeVyplneno(vstupJmeno_, false)) {
        pacId_ = Databaze::ziskatIdPacienta(vstupPrijmeni_->text(), vstupJmeno_->text());

        povolitVyhledavaciTlacitko(pacId_);
        zpristupnitVstupy(!pacId_);
        tlacitkoJinyPacient_->setEnabled(pacId_);

        if (!pacId_) {
            validator_->zobrazitUpozorneni(QString("Pacient " + Text::kurziva(vstupPrijmeni_->text(), vstupJmeno_->text())
                                                   + " nebyl nalezen v databázi"));
        }
    }
    else {
        validator_->zobrazitUpozorneni(QString("Políčka " + Text::kurziva("Příjmení")
                                               + " a " + Text::kurziva("Jméno")
                                               + " musí být vyplněna"));
    }
}

void DialogPrehledZavozuProPacienta::zrusitVolbuPacienta() {
        zpristupnitVstupy(true);
        povolitVyhledavaciTlacitko(false);
        vstupPrijmeni_->clear();
        vstupJmeno_->clear();
        vstupPrijmeni_->setFocus();

        zpristupnitTabulku(false);
}

void DialogPrehledZavozuProPacienta::zpristupnitVstupy(bool zpristupnit) {
    foreach (QLineEdit *policko, QList<QLineEdit*>({vstupPrijmeni_, vstupJmeno_})) {
        policko->setReadOnly(!zpristupnit);
        policko->setFrame(zpristupnit);
        policko->setStyleSheet(zpristupnit ? ""
                                           : "background-color: transparent; font-weight: bold");
        zpristupnit ? policko->installEventFilter(validator_)
                    : policko->removeEventFilter(validator_);
    }

    tlacitkoNajitVDatabazi_->setVisible(zpristupnit);
    tlacitkoJinyPacient_->setVisible(!zpristupnit);
}

bool DialogPrehledZavozuProPacienta::vyhledatZaznamy() {
    return Databaze::prehledZavozuProPacienta(pacId_, datumOd_, datumDo_, queryModel_);
}

QString DialogPrehledZavozuProPacienta::navrhnoutNazevCsvSouboru() {
    return QString("závozy pro " + vstupPrijmeni_->text() + " " + vstupJmeno_->text().at(0) + ". "
                   + datumOd_.toString(Qt::ISODate) + " - " + datumDo_.toString(Qt::ISODate) + ".csv");
}
