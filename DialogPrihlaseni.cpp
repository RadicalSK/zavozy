#include "DialogPrihlaseni.h"
#include "ui_DialogPrihlaseni.h"

DialogPrihlaseni::DialogPrihlaseni(Uzivatel *&prihlasenyUzivatel, QWidget *rodic) :
    QDialog(rodic), prihlasenyUzivatel_(prihlasenyUzivatel),
    ui(new Ui::DialogPrihlaseni)
{
    ui->setupUi(this);

    validator_ = new ValidatorVstupu(ui->popisekUpozorneni);
    validator_->nastavitFiltrUdalosti(QList<QLineEdit*>({ui->vstupUzivatel, ui->vstupHeslo}));
}

DialogPrihlaseni::~DialogPrihlaseni() {
    delete ui;
    delete validator_;
}

bool DialogPrihlaseni::otevrit(Uzivatel *&prihlasenyUzivatel, QWidget *rodic) {
    DialogPrihlaseni *dialog = new DialogPrihlaseni(prihlasenyUzivatel, rodic);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    return dialog->exec();
}

void DialogPrihlaseni::on_vstupUzivatel_textChanged(const QString &text) {
    ui->vstupUzivatel->setText(text.toUpper());
}

void DialogPrihlaseni::on_tlacitkoZrusit_clicked() {
    reject();
}

void DialogPrihlaseni::on_tlacitkoPrihlasit_clicked() {
    bool jmenoVyplneno = validator_->jeVyplneno(ui->vstupUzivatel);
    bool hesloVyplneno = validator_->jeVyplneno(ui->vstupHeslo);

    if (!jmenoVyplneno || !hesloVyplneno) {
        validator_->zobrazitUpozorneni("Vyplňte jméno i heslo");
    }
    else {
        if (Databaze::vyhledatAktivnihoUzivatele(ui->vstupUzivatel->text(), ui->vstupHeslo->text(), prihlasenyUzivatel_)) {
            accept();
        }
        else {
            bool jeAktivni;
            if (Databaze::existujeUzivatel(ui->vstupUzivatel->text(), jeAktivni)) {
                jeAktivni ? validator_->zobrazitUpozorneni(ui->vstupHeslo, false, "Nesprávné heslo")
                          : validator_->zobrazitUpozorneni(ui->vstupUzivatel, false, "Uživatel byl deaktivován administrátorem");
            }
            else {
                validator_->zobrazitUpozorneni(ui->vstupUzivatel, false, "Neexistující uživatel");
            }
        }
    }
}

void DialogPrihlaseni::keyPressEvent(QKeyEvent *udalost) {
    // Ctrl+Enter prihlasi uzivatele
    if ((udalost->key() == Qt::Key_Enter || udalost->key() == Qt::Key_Return)
            && (udalost->modifiers().testFlag(Qt::ControlModifier))) {
        ui->tlacitkoPrihlasit->click();
    }
    else {
        QDialog::keyPressEvent(udalost);
    }
}
