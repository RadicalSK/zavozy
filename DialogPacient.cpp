#include "DialogPacient.h"
#include "ui_DialogPacient.h"

DialogPacient::DialogPacient(TypDialogu typ, Pacient *upravovanyPacient, QWidget *rodic) :
    QDialog(rodic), aktualniPacient_(upravovanyPacient), typDialogu_(typ), naseptavac_(new Naseptavac),
    ui(new Ui::DialogPacient)
{
    ui->setupUi(this);

    vstupyPrijmeniAJmeno_ << ui->vstupPrijmeni << ui->vstupJmeno;
    ostatniVstupy_ << ui->vstupUlice << ui->vstupObec
                   << ui->vstupTelefon << ui->vstupPoznTel
                   << ui->vstupTelefon2 << ui->vstupPoznTel2 << ui->vstupPoznamka;
    povinneVstupy_ << vstupyPrijmeniAJmeno_ << ui->vstupUlice << ui->vstupObec << ui->vstupTelefon;
    vstupyTelefonu_ << ui->vstupTelefon << ui->vstupPoznTel
                    << ui->vstupTelefon2 << ui->vstupPoznTel2;

    validator_ = new ValidatorVstupu(ui->popisekUpozorneni);
    validator_->nastavitFiltrUdalosti(vstupyPrijmeniAJmeno_);
    validator_->nastavitFiltrUdalosti(ostatniVstupy_);

    vstupyPrijmeniAJmeno_ << ui->vstupPrijmeni << ui->vstupJmeno;
    ostatniVstupy_ << ui->vstupUlice << ui->vstupObec
                   << ui->vstupTelefon << ui->vstupPoznTel
                   << ui->vstupTelefon2 << ui->vstupPoznTel2 << ui->vstupPoznamka;
    povinneVstupy_ << vstupyPrijmeniAJmeno_ << ui->vstupUlice << ui->vstupObec << ui->vstupTelefon;
    vstupyTelefonu_ << ui->vstupTelefon << ui->vstupPoznTel
                    << ui->vstupTelefon2 << ui->vstupPoznTel2;

    ui->vstupObec->setCompleter(naseptavac_->naseptavacObci());

    switch (typDialogu_) {
        case NovyPacient: {
            this->setWindowTitle("Nový pacient");
            ui->volbaUpravitJmeno->setVisible(false);

        } break;
        case UpravaPacienta: {
            this->setWindowTitle("Úprava pacienta");
            // pokud se neupravuje aktualni pacient, umozni vyhledat libovolneho pacienta
            if (!aktualniPacient_) {
                pripravitVyhledavaniPacienta();
            }
        } break;
    }
}

DialogPacient::DialogPacient(QString prijmeni, QString jmeno, QWidget *rodic)
    : DialogPacient(TypDialogu::NovyPacient, nullptr, rodic)
{
    ui->vstupPrijmeni->setText(prijmeni);
    ui->vstupJmeno->setText(jmeno);
    ui->vstupUlice->setFocus();
}

DialogPacient::DialogPacient(Pacient *upravovanyPacient, QWidget *rodic)
    : DialogPacient(TypDialogu::UpravaPacienta, upravovanyPacient, rodic)
{
    // vytvori pracovni kopii aktualniho pacienta
    pacient_ = new Pacient(*upravovanyPacient);
    nacistPacientaKUprave();
}

DialogPacient::~DialogPacient() {
    delete ui;
    delete naseptavac_;
    delete validator_;
    delete pacient_;
}

bool DialogPacient::otevritNovyPacient(QWidget *rodic) {
    DialogPacient *dialog = new DialogPacient(DialogPacient::NovyPacient, nullptr, rodic);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    return dialog->exec();
}

bool DialogPacient::otevritNovyPacient(QString prijmeni, QString jmeno, QWidget *rodic) {
    DialogPacient *dialog = new DialogPacient(prijmeni, jmeno, rodic);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    return dialog->exec();
}

bool DialogPacient::otevritUpravaPacienta(QWidget *rodic) {
    DialogPacient *dialog = new DialogPacient(DialogPacient::UpravaPacienta, nullptr, rodic);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    return dialog->exec();
}

bool DialogPacient::otevritUpravaPacienta(Pacient *upravovanyPacient, QWidget *rodic) {
    DialogPacient *dialog = new DialogPacient(upravovanyPacient, rodic);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    return dialog->exec();
}

void DialogPacient::on_vstupPrijmeni_textChanged(const QString &text) {
    Q_UNUSED(text)
    // pouze pri vyhledavani pacienta k uprave
    if (rezimVyhledavaniPacienta_) {
        ui->vstupJmeno->clear();
        ui->vstupJmeno->setEnabled(false);
    }
}

void DialogPacient::on_vstupPrijmeni_editingFinished() {
    validovatVstupPrijmeni();
}

void DialogPacient::on_vstupPrijmeni_returnPressed() {
    // pri vyhledavani pacienta k uprave se nastavi naseptavac jmen zodpovidajici zvolenemu prijmeni
    if (rezimVyhledavaniPacienta_ && validovatVstupPrijmeni()) {
        ui->vstupJmeno->setEnabled(true);
        ui->vstupJmeno->setCompleter(naseptavac_->naseptavacJmen(ui->vstupPrijmeni->text()));
    }
}

void DialogPacient::on_vstupJmeno_editingFinished() {
    validovatVstupJmeno();
}

void DialogPacient::on_vstupJmeno_returnPressed() {    
    if (rezimVyhledavaniPacienta_) {
        vyhledatPacientaKUprave();
    }
}

void DialogPacient::on_volbaUpravitJmeno_clicked(bool zvoleno) {
    zpristupnitVstupyPrijmeniAJmeno(zvoleno);

    if (zvoleno) {
        OknoZprav::upozorneni("Změna jména se promítne zpětně do všech záznamů daného pacienta v databázi.",
                              "Použijte pouze k opravě překlepu ve jménu/příjmení nebo při změně příjmení u pacientky!");
        ui->vstupPrijmeni->setFocus();
    }
    else {
        ui->vstupPrijmeni->setText(pacient_->prijmeni());
        ui->vstupJmeno->setText(pacient_->jmeno());
        ui->vstupUlice->setFocus();
    }
}

void DialogPacient::on_vstupUlice_editingFinished() {
    validovatVstupUlice();
}

void DialogPacient::on_vstupObec_editingFinished() {
    validovatVstupObec();
}

void DialogPacient::on_vstupTelefon_editingFinished() {
    validovatVstupTelefon();
}

void DialogPacient::on_volbaNemaTelefon_toggled(bool zvoleno) {
    zpristupnitVstupyTelefonu(zvoleno);
}

void DialogPacient::on_vstupPoznTel_editingFinished() {
    validovatVstupPoznTel();
}

void DialogPacient::on_vstupTelefon2_editingFinished() {
    validovatVstupTelefon2();
}

void DialogPacient::on_vstupPoznTel2_editingFinished() {
    validovatVstupPoznTel2();
}

void DialogPacient::on_tlacitkoZrusit_clicked() {
    reject();
}

void DialogPacient::on_tlacitkoUlozit_clicked() {
    bool povinnePolozkyValidni = validovatVstupPrijmeni() && validovatVstupJmeno()
            && validovatVstupUlice() && validovatVstupObec() && validovatVstupTelefon();

    if (povinnePolozkyValidni) {
        bool nepovinnePolozkyValidni = validovatVstupPoznTel() && validovatVstupTelefon2() && validovatVstupPoznTel2();
        if (nepovinnePolozkyValidni) {
            switch (typDialogu_) {
                case NovyPacient:
                    zalozitNovehoPacienta();
                    break;
                case UpravaPacienta:
                    upravitStavajicihoPacienta();
                    break;
            }
        }
    }
    else {
        validator_->zobrazitUpozorneni(povinneVstupy_, "Vyplňte korektně všechny povinné položky");
    }
}

void DialogPacient::pripravitVyhledavaniPacienta() {
    ui->vstupPrijmeni->setPlaceholderText("Zadejte příjmení stávajícího pacienta");
    ui->vstupJmeno->setPlaceholderText("Zadejte jméno stávajícího pacienta");
    ui->vstupJmeno->setEnabled(false);
    zpristupnitOstatniVstupy(false);
    zpristupnitVolbuNemaTelefon(false);
    ui->vstupPrijmeni->setCompleter(naseptavac_->naseptavacPrijmeni());
    rezimVyhledavaniPacienta_ = true;
}

void DialogPacient::nacistPacientaKUprave() {
    rezimVyhledavaniPacienta_ = false;

    // zrusit naseptavace
    ui->vstupPrijmeni->setCompleter(nullptr);
    ui->vstupJmeno->setCompleter(nullptr);

    // pripravit vstupy
    zobrazitIdPacienta(pacient_);
    naplnitVstupyUdaji(pacient_);
    zpristupnitVstupyPrijmeniAJmeno(false);
    zpristupnitOstatniVstupy(true);
    if (pacient_->telefon1().isEmpty()) {
        ui->volbaNemaTelefon->setChecked(true);
        zpristupnitVolbuNemaTelefon(true);
    }

    ui->vstupUlice->setFocus();
}

void DialogPacient::zpristupnitVstupyPrijmeniAJmeno(bool zpristupnit) {
    foreach (QLineEdit *policko, vstupyPrijmeniAJmeno_) {
        policko->setReadOnly(!zpristupnit);
        policko->setFrame(zpristupnit);
        policko->setStyleSheet(zpristupnit ? ""
                                           : "background-color: transparent; font-weight: bold");
        zpristupnit ? policko->installEventFilter(validator_)
                    : policko->removeEventFilter(validator_);
    }
}

void DialogPacient::zpristupnitOstatniVstupy(bool zpristupnit) {
    foreach (QLineEdit *policko, ostatniVstupy_) {
        policko->setEnabled(zpristupnit);
    }
    ui->volbaUpravitJmeno->setEnabled(zpristupnit);
}

void DialogPacient::zpristupnitVolbuNemaTelefon(bool zpristupnit) {
    ui->volbaNemaTelefon->setEnabled(zpristupnit);
}

void DialogPacient::zpristupnitVstupyTelefonu(bool zpristupnit) {
    foreach (QLineEdit *policko, vstupyTelefonu_) {
        validator_->smazatUpozorneni(policko);
        policko->setEnabled(!zpristupnit);
        policko->clear();
    }

    if (zpristupnit)
        ui->vstupPoznamka->setFocus();
    else
        ui->vstupTelefon->setFocus();
}

void DialogPacient::zobrazitIdPacienta(Pacient *pacient, bool zobrazit) {
    if (zobrazit) {
        ui->textIdPacienta->setText("ID pacienta");
        ui->popisekIdPacienta->setText(QString::number(pacient->id()));
    }
    else {
        ui->textIdPacienta->clear();
        ui->popisekIdPacienta->clear();
    }
}

void DialogPacient::naplnitVstupyUdaji(Pacient *pacient) {
    ui->vstupPrijmeni->setText(pacient->prijmeni());
    ui->vstupJmeno->setText(pacient->jmeno());
    ui->vstupUlice->setText(pacient->ulice());
    ui->vstupObec->setText(pacient->obec());
    ui->vstupTelefon->setText(pacient->telefon1());
    ui->vstupPoznTel->setText(pacient->poznTel1());
    ui->vstupTelefon2->setText(pacient->telefon2());
    ui->vstupPoznTel2->setText(pacient->poznTel2());
    ui->vstupPoznamka->setText(pacient->poznamka());
}

bool DialogPacient::validovatVstupPrijmeni() {
    return validator_->jeVyplneno(ui->vstupPrijmeni);
}

bool DialogPacient::validovatVstupJmeno() {
    return validator_->jeVyplneno(ui->vstupJmeno);
}

bool DialogPacient::validovatVstupUlice() {
    return validator_->jeVyplneno(ui->vstupUlice);
}

bool DialogPacient::validovatVstupObec() {
    if (validator_->jeVyplneno(ui->vstupObec))
        return validator_->obsahujePsc(ui->vstupObec);
    else
        return false;
}

bool DialogPacient::validovatVstupTelefon() {
    if (ui->volbaNemaTelefon->isChecked())
        return true;
    else
        return validator_->obsahujeTelCislo(ui->vstupTelefon);
}

bool DialogPacient::validovatVstupPoznTel() {
    if (ui->vstupPoznTel->text().isEmpty())
        return true;
    else {
        if (validovatVstupTelefon())
            return true;
        else {
            validator_->zobrazitUpozorneni(ui->vstupPoznTel,
                                           false,
                                           "Nelze uložit poznámku k telefonu, pokud není (správně) vyplněn");
            return false;
        }
    }
}

bool DialogPacient::validovatVstupTelefon2() {
    if (ui->volbaNemaTelefon->isChecked()) {
        return true;
    }
    else
        return validator_->obsahujeTelCisloNeboJePrazdne(ui->vstupTelefon2);
}

bool DialogPacient::validovatVstupPoznTel2() {
    if (ui->vstupPoznTel2->text().isEmpty())
        return true;
    else {
        if (validovatVstupTelefon2()) {
            if (ui->vstupTelefon2->text().isEmpty()) {
                validator_->zobrazitUpozorneni(ui->vstupPoznTel2,
                                               false,
                                               "Nelze uložit poznámku k telefonu 2, pokud není vyplněn");
                return false;
            }
            else
                return true;
        }
        else {
            validator_->zobrazitUpozorneni(ui->vstupPoznTel2,
                                           false,
                                           "Nelze uložit poznámku k telefonu 2, pokud není správně vyplněn");
            return false;
        }
    }
}

bool DialogPacient::prijmeniNeboJmenoZmeneno() {
    return ((ui->vstupPrijmeni->text() != pacient_->prijmeni())
            || (ui->vstupJmeno->text() != pacient_->jmeno()));
}

void DialogPacient::vyhledatPacientaKUprave() {
    QString prijmeni = ui->vstupPrijmeni->text();
    QString jmeno = ui->vstupJmeno->text();

    if (Databaze::vyhledatPacienta(prijmeni, jmeno, pacient_)) {
        nacistPacientaKUprave();
    }
    else {
        validator_->zobrazitUpozorneni(QString("Pacient " + Text::kurziva(prijmeni, jmeno)
                                               + " nebyl nalezen v databázi"));
    }
}

bool DialogPacient::existujePacientSeStejnymJmenem() {
    Pacient *jmenovec;
    if (Databaze::vyhledatPacienta(ui->vstupPrijmeni->text(),
                                   ui->vstupJmeno->text(),
                                   jmenovec)) {
        chybovaHlaskaDuplicita(jmenovec);
        delete jmenovec;    // smaze pomocny objekt pacienta vytvoreny pri vyhledavani
        return true;
    }

    return false;
}

void DialogPacient::zalozitNovehoPacienta() {
    if (existujePacientSeStejnymJmenem()) {
        validator_->zobrazitUpozorneni(vstupyPrijmeniAJmeno_, "Pokus o duplicitní záznam");
    }
    else {
        pacient_ = new Pacient(0, ui->vstupPrijmeni->text(), ui->vstupJmeno->text(),
                               ui->vstupUlice->text(), ui->vstupObec->text(),
                               Text::odstranitBileZnaky(ui->vstupTelefon->text()), ui->vstupPoznTel->text(),
                               Text::odstranitBileZnaky(ui->vstupTelefon2->text()), ui->vstupPoznTel2->text(),
                               ui->vstupPoznamka->text());
        if (Databaze::zalozitNovehoPacienta(pacient_)) {
            accept();
        }
        else {
            OknoZprav::chybaPriZapisuDoDb("Pacient nebyl založen.");
        }
    }
}

void DialogPacient::upravitStavajicihoPacienta() {
    // pokud doslo ke zmene jmena, je nutne zkontrolovat duplicitu
    if (prijmeniNeboJmenoZmeneno()) {
        if (existujePacientSeStejnymJmenem()) {
            validator_->zobrazitUpozorneni(vstupyPrijmeniAJmeno_, "Pokus o duplicitní záznam");
            return;
        }
        else {
            pacient_->setPrijmeni(ui->vstupPrijmeni->text());
            pacient_->setJmeno(ui->vstupJmeno->text());
        }
    }

    // jestli se neupravuje jmeno nebo upravene jmeno nekoliduje s jinym pacientem, stavajici pacient se upravi
    pacient_->setUlice(ui->vstupUlice->text());
    pacient_->setObec(ui->vstupObec->text());
    pacient_->setTelefon1(Text::odstranitBileZnaky(ui->vstupTelefon->text()));
    pacient_->setPoznTel1(ui->vstupPoznTel->text());
    pacient_->setTelefon2(Text::odstranitBileZnaky(ui->vstupTelefon2->text()));
    pacient_->setPoznTel2(ui->vstupPoznTel2->text());
    pacient_->setPoznamka(ui->vstupPoznamka->text());

    // upravi se pacientuv zaznam v databazi
    if (Databaze::upravitStavajicihoPacienta(pacient_)) {
        // pokud je, aktualizuje se i aktualne nacteny pacient
        if (aktualniPacient_) {
            *aktualniPacient_ = *pacient_;
        }
        accept();
    }
    else {
        OknoZprav::chybaPriZapisuDoDb("Pacient nebyl upraven.");
    }
}

void DialogPacient::chybovaHlaskaDuplicita(Pacient *jmenovec) {
    QString zprava = "Uživatel s uvedenou kombinací příjmení a jména už v databázi existuje:\n  ";
    zprava += jmenovec->prijmeni() += QString(" ")
            += jmenovec->jmeno() += QString("\n  adresa: ")
            += jmenovec->adresa() += QString("\n  telefon: ")
            += jmenovec->telefony();

    QString doplnujiciText = "Jestli se jedná o jinou osobu, připojte k jejímu jménu "
                      "pro odlišení rok narození (př. \"Petr 1954\") a zkuste znova. "
                      "Následně upravte jmenovce obdobným způsobem pro jasné rozlišení.";

    OknoZprav::upozorneni(zprava, doplnujiciText);
}

void DialogPacient::keyPressEvent(QKeyEvent *udalost) {
    // Ctrl+Enter zalozi pacienta
    if ((udalost->key() == Qt::Key_Enter || udalost->key() == Qt::Key_Return)
            && (udalost->modifiers().testFlag(Qt::ControlModifier))) {
        ui->tlacitkoUlozit->click();
    }
    else {
        QDialog::keyPressEvent(udalost);
    }
}
