#include "DialogUpravitSmazatUzivatele.h"
#include "ui_DialogUpravitSmazatUzivatele.h"

DialogUpravitSmazatUzivatele::DialogUpravitSmazatUzivatele(Uzivatel *uzivatel, bool zmenaHesla, QWidget *rodic) :
    QDialog(rodic), uzivatel_(uzivatel), zmenaHesla_(zmenaHesla),
    ui(new Ui::DialogUpravitSmazatUzivatele)
{
    ui->setupUi(this);

    vstupyHesel_ << ui->vstupStareHeslo << ui->vstupNoveHeslo << ui->vstupNoveHeslo2;

    validator_ = new ValidatorVstupu(ui->popisekUpozorneni);
    validator_->nastavitFiltrUdalosti(vstupyHesel_);

    // při změně hesla jsou přístupná pouze políčka pro hesla, jiné vlastnosti uživatele nelze měnit
    if (zmenaHesla) {
        this->setWindowTitle("Změna hesla");
        ui->seznamUzivJmeno->addItem(uzivatel->uzivJmeno());
        ui->vstupCeleJmeno->setText(uzivatel->celeJmeno());
        ui->vstupNoveHeslo->setPlaceholderText(QString("minimálně " + QString::number(Heslo::minDelka) + " znaků"));
    }
    // v opačném případě může admin změnit údaje jiným uživatelům nebo je smazat
    // sám sebe nemůže smazat ani se zbavit administrátorských práv
    else {
        // vytvoř seznam uživatelů
        foreach (QString uzivJmeno, Databaze::seznamUzivatelu()) {
            ui->seznamUzivJmeno->addItem(uzivJmeno);
        }

        ui->seznamUzivJmeno->setEnabled(true);
        ui->seznamUzivJmeno->setCurrentText(uzivatel->uzivJmeno());
        ui->vstupCeleJmeno->setEnabled(true);
        ui->vstupCeleJmeno->setText(uzivatel->celeJmeno());
        ui->textAdmin->setEnabled(true);
        ui->volbaAdmin->setChecked(uzivatel->admin());
        ui->vstupStareHeslo->setEnabled(false);
        ui->vstupNoveHeslo->setPlaceholderText("obnova zapomenutého hesla");
        ui->vstupNoveHeslo2->setPlaceholderText("jinak nevyplňovat");
    }
}

DialogUpravitSmazatUzivatele::~DialogUpravitSmazatUzivatele() {
    delete ui;
    delete validator_;
}

bool DialogUpravitSmazatUzivatele::otevritZmenaHesla(Uzivatel *uzivatel) {
    DialogUpravitSmazatUzivatele *dialog = new DialogUpravitSmazatUzivatele(uzivatel, true);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    return dialog->exec();
}

bool DialogUpravitSmazatUzivatele::otevritUpravaUzivatele(Uzivatel *uzivatel) {
    DialogUpravitSmazatUzivatele *dialog = new DialogUpravitSmazatUzivatele(uzivatel, false);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    return dialog->exec();
}

void DialogUpravitSmazatUzivatele::on_seznamUzivJmeno_currentIndexChanged(const QString &uzivJmeno) {
    validator_->smazatUpozorneni(ui->vstupCeleJmeno);
    validator_->smazatUpozorneni(ui->vstupNoveHeslo);
    validator_->smazatUpozorneni(ui->vstupNoveHeslo2);
    ui->vstupNoveHeslo->setText("");
    ui->vstupNoveHeslo2->setText("");

    QString celeJmeno;
    bool admin;

    Databaze::nacistUdajeUzivatele(uzivJmeno, celeJmeno, admin);

    ui->vstupCeleJmeno->setText(celeJmeno);
    ui->volbaAdmin->setChecked(admin);

    // admin nesmí mazat nebo zbavovat administrátorských práv sebe sama
    bool jeSamouprava = (uzivJmeno == uzivatel_->uzivJmeno());
    ui->volbaAdmin->setEnabled(!jeSamouprava);
    ui->volbaSmazatUzivatele->setEnabled(!jeSamouprava);
}

void DialogUpravitSmazatUzivatele::on_vstupCeleJmeno_editingFinished() {
    validovatVstupCeleJmeno();
}

void DialogUpravitSmazatUzivatele::on_vstupStareHeslo_editingFinished() {
    validovatVstupStareHeslo();
}

void DialogUpravitSmazatUzivatele::on_vstupNoveHeslo_editingFinished() {
    validovatVstupNoveHeslo(ui->vstupNoveHeslo);
}

void DialogUpravitSmazatUzivatele::on_vstupNoveHeslo2_editingFinished() {
    validovatVstupNoveHeslo(ui->vstupNoveHeslo2);
}

void DialogUpravitSmazatUzivatele::on_volbaSmazatUzivatele_toggled(bool checked) {
    if (checked) {
        bool smazat = OknoZprav::potvrzeni(OknoZprav::Varovani, "Zvolený uživatel bude smazán.",
                                           "Uživatel nebude víc oprávněn používat aplikaci, "
                                           "ale veškeré záznamy o jeho činnosti zůstanou uložené v databázi. "
                                           "Jeho uživatelské jméno nebude možné opětovně použít.");
        if (!smazat) {
            ui->volbaSmazatUzivatele->setChecked(false);
        }
    }
}

void DialogUpravitSmazatUzivatele::on_tlacitkoZrusit_clicked() {
    reject();
}

void DialogUpravitSmazatUzivatele::on_tlacitkoUlozit_clicked() {
    if (zmenaHesla_) {  // změna vlastního hesla uživatelem
        QString stareHeslo = uzivatel_->hesloSha256Hash();
        QString zadaneHeslo = Heslo::sha256Hash(ui->vstupStareHeslo->text());

        if (stareHeslo != zadaneHeslo) {
            validator_->zobrazitUpozorneni(ui->vstupStareHeslo, true, "není správne vyplněno");
        }
        else if (!validovatVstupNoveHeslo(ui->vstupNoveHeslo) || !validovatVstupNoveHeslo(ui->vstupNoveHeslo2)) {
            return;
        }
        else if (!validator_->shodujiSeHesla(ui->vstupNoveHeslo, ui->vstupNoveHeslo2)) {
            return;
        }
        else {
            if (Databaze::zmenitHesloUzivatele(uzivatel_->uzivJmeno(), ui->vstupNoveHeslo->text())) {
                uzivatel_->setHesloSha256Hash(ui->vstupNoveHeslo->text());
                accept();
            }
            else {
                OknoZprav::chybaPriZapisuDoDb("Heslo nebylo změněno.");
            }
        }
    }
    else {  // změna nebo smazání jiného uživatele administrátorem
        // smazání
        if (ui->volbaSmazatUzivatele->isChecked()) {
            if (Databaze::smazatUzivatele(ui->seznamUzivJmeno->currentText())) {
                accept();
            }
            else {
                OknoZprav::chybaPriZapisuDoDb("Uživatel nebyl smazán.");
            }
        }
        // změna
        else if (validovatVstupCeleJmeno()) {
            // beze změny hesla
            if (ui->vstupNoveHeslo->text().isEmpty() && ui->vstupNoveHeslo2->text().isEmpty()) {
                if (Databaze::upravitUzivatele(ui->seznamUzivJmeno->currentText(),
                                               ui->vstupCeleJmeno->text(), ui->volbaAdmin->isChecked())) {
                    accept();
                }
                else {
                    OknoZprav::chybaPriZapisuDoDb("Uživatel nebyl upraven.");
                }
            }
            // se změnou hesla
            else {
                if (!validovatVstupNoveHeslo(ui->vstupNoveHeslo) || !validovatVstupNoveHeslo(ui->vstupNoveHeslo2)) {
                    return;
                }
                else if (!validator_->shodujiSeHesla(ui->vstupNoveHeslo, ui->vstupNoveHeslo2)) {
                    return;
                }
                else {
                    if (Databaze::zmenitHesloUzivatele(ui->seznamUzivJmeno->currentText(), ui->vstupNoveHeslo->text())) {
                        // jestli admin meni heslo sobe, je potreba ho aktualizovat v aplikaci, nejen v databazi
                        if (ui->seznamUzivJmeno->currentText() == uzivatel_->uzivJmeno()) {
                            uzivatel_->setHesloSha256Hash(ui->vstupNoveHeslo->text());
                        }
                        accept();
                    }
                    else {
                        OknoZprav::chybaPriZapisuDoDb("Uživatel nebyl upraven.");
                    }
                }
            }
        }
    }
}

bool DialogUpravitSmazatUzivatele::validovatVstupCeleJmeno() {
    return validator_->obsahujeMinPocetSlov(ui->vstupCeleJmeno, 2);
}

bool DialogUpravitSmazatUzivatele::validovatVstupStareHeslo() {
    return validator_->jeVyplneno(ui->vstupStareHeslo);
}

bool DialogUpravitSmazatUzivatele::validovatVstupNoveHeslo(QLineEdit *policko) {
    if (zmenaHesla_) {
        return validator_->dosahujeMinDelku(policko, Heslo::minDelka);
    }
    else {
        return validator_->dosahujeMinDelkuNeboJePrazdne(policko, Heslo::minDelka, true);
    }
}

void DialogUpravitSmazatUzivatele::keyPressEvent(QKeyEvent *udalost) {
    // Ctrl+Enter ulozi zmeny
    if ((udalost->key() == Qt::Key_Enter || udalost->key() == Qt::Key_Return)
            && (udalost->modifiers().testFlag(Qt::ControlModifier))) {
        ui->tlacitkoUlozit->click();
    }
    else {
        QDialog::keyPressEvent(udalost);
    }
}
