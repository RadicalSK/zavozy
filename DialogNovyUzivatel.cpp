#include "DialogNovyUzivatel.h"
#include "ui_DialogNovyUzivatel.h"

DialogNovyUzivatel::DialogNovyUzivatel(QWidget *rodic) :
    QDialog(rodic), ui(new Ui::DialogNovyUzivatel)
{
    ui->setupUi(this);

    vsechnaPolicka_ << ui->vstupUzivJmeno << ui->vstupCeleJmeno << ui->vstupHeslo << ui->vstupHeslo2;

    validator_ = new ValidatorVstupu(ui->popisekUpozorneni);
    validator_->nastavitFiltrUdalosti(vsechnaPolicka_);

    // prvni uzivatel musi byt admin
    if (Databaze::pocetAktivnichUzivatelu() == 0) {
        ui->volbaBezny->setEnabled(false);
        ui->volbaAdmin->setChecked(true);
    }

    ui->vstupHeslo->setPlaceholderText(QString("minimálně " + QString::number(Heslo::minDelka) + " znaky"));
}

DialogNovyUzivatel::~DialogNovyUzivatel() {
    delete ui;
    delete validator_;
}

bool DialogNovyUzivatel::otevrit() {
    DialogNovyUzivatel *dialog = new DialogNovyUzivatel();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    return dialog->exec();
}

void DialogNovyUzivatel::on_vstupUzivJmeno_textChanged(const QString &text) {
    ui->vstupUzivJmeno->setText(text.toUpper());
}

void DialogNovyUzivatel::on_vstupUzivJmeno_editingFinished() {
    validovatVstupUzivJmeno();
}

void DialogNovyUzivatel::on_vstupCeleJmeno_editingFinished() {
    validovatVstupCeleJmeno();
}

void DialogNovyUzivatel::on_vstupHeslo_editingFinished() {
    validovatVstupHeslo();
}

void DialogNovyUzivatel::on_vstupHeslo2_editingFinished() {
    validovatVstupHeslo2();
}

void DialogNovyUzivatel::on_tlacitkoZrusit_clicked() {
    reject();
}

void DialogNovyUzivatel::on_tlacitkoZalozit_clicked() {
    if (validovatVstupUzivJmeno() && validovatVstupCeleJmeno() && validovatVstupHeslo() && validovatVstupHeslo2()) {
        if (!jeZvolenTypUzivatele()) {
            validator_->zobrazitUpozorneni("Zvolte typ uživatele");
        }
        else if (!validator_->shodujiSeHesla(ui->vstupHeslo, ui->vstupHeslo2)) {
            return;
        }
        else {
            if (!Databaze::existujeUzivatel(ui->vstupUzivJmeno->text())) {
                if (Databaze::zalozitNovehoUzivatele(ui->vstupUzivJmeno->text(), ui->vstupHeslo->text(),
                                                     ui->vstupCeleJmeno->text(), ui->volbaAdmin->isChecked())) {
                    accept();
                }
                else {
                    OknoZprav::chybaPriZapisuDoDb("Uživatel nebyl založen.");
                }
            }
            else {
                validator_->zobrazitUpozorneni(ui->vstupUzivJmeno, false, "Uživatelské jméno se již používá");
            }
        }
    }
    else {
        validator_->zobrazitUpozorneni(vsechnaPolicka_, "Vyplňte korektně všechna políčka");
    }
}

bool DialogNovyUzivatel::validovatVstupUzivJmeno() {
    return validator_->dosahujeMinDelku(ui->vstupUzivJmeno, 3, true);
}

bool DialogNovyUzivatel::validovatVstupCeleJmeno() {
    return validator_->obsahujeMinPocetSlov(ui->vstupCeleJmeno, 2, true);
}

bool DialogNovyUzivatel::validovatVstupHeslo() {
    return validator_->dosahujeMinDelku(ui->vstupHeslo, Heslo::minDelka, true);
}

bool DialogNovyUzivatel::validovatVstupHeslo2() {
    return validator_->dosahujeMinDelku(ui->vstupHeslo2, Heslo::minDelka, true);
}

bool DialogNovyUzivatel::jeZvolenTypUzivatele() {
    return (ui->volbaAdmin->isChecked() || ui->volbaBezny->isChecked());
}

void DialogNovyUzivatel::keyPressEvent(QKeyEvent *udalost) {
    // Ctrl+Enter zalozi uzivatele
    if ((udalost->key() == Qt::Key_Enter || udalost->key() == Qt::Key_Return)
            && (udalost->modifiers().testFlag(Qt::ControlModifier))) {
        ui->tlacitkoZalozit->click();
    }
    else {
        QDialog::keyPressEvent(udalost);
    }
}
