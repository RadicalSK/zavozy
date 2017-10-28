#include "DialogSeznamPruvodek.h"
#include "ui_DialogSeznamPruvodek.h"

DialogSeznamPruvodek::DialogSeznamPruvodek(QList<Zavoz> *dnesVytistenePruvodky, QList<Zavoz> *nevytistenePruvodky, QWidget *rodic)
    : QDialog(rodic), p_dnesVytistenePruvodky_(dnesVytistenePruvodky), p_nevytistenePruvodky_(nevytistenePruvodky),
      ui(new Ui::DialogSeznamPruvodek)
{
    ui->setupUi(this);

    if (dnesVytistenePruvodky) {    // opetovne zarazeni vytistenych do tiskove fronty
        if (naplnitSeznamPruvodek(p_dnesVytistenePruvodky_, ui->seznamDnesVytistene)) {
            ui->seznamDnesVytistene->setSelectionMode(QAbstractItemView::SingleSelection);
        }
        this->setWindowTitle("Přehled vytištěných průvodek");
    }
    else  {   // pouze prehled nevytistenych
        ui->textDnesVytistene->setVisible(false);
        ui->seznamDnesVytistene->setVisible(false);
        ui->sekceZaraditFronty->setVisible(false);
        ui->textPruvodkyVeFronte->setVisible(false);

        ui->tlacitkoPotvrdit->setEnabled(false);

        this->resize(this->width(), this->height()/2);
        this->setWindowTitle("Průvodky v tiskové frontě");
    }

    naplnitSeznamPruvodek(p_nevytistenePruvodky_, ui->seznamPruvodkyVeFronte);
}

bool DialogSeznamPruvodek::otevritNevytistene(QList<Zavoz> *nevytistenePruvodky) {
    DialogSeznamPruvodek *dialog = new DialogSeznamPruvodek(nullptr, nevytistenePruvodky);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    return dialog->exec();
}

bool DialogSeznamPruvodek::otevritOpetovneZarazeniDoFronty(QList<Zavoz> *dnesVytistenePruvodky, QList<Zavoz> *nevytistenePruvodky) {
    DialogSeznamPruvodek *dialog = new DialogSeznamPruvodek(dnesVytistenePruvodky, nevytistenePruvodky);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    return dialog->exec();
}

void DialogSeznamPruvodek::on_seznamDnesVytistene_currentRowChanged(int radek) {
    if (ui->seznamDnesVytistene->styleSheet() != "color: grey;") {
        aktivniRadek_ = radek;
        ui->tlacitkoZaraditDoFronty->setEnabled(aktivniRadek_ > -1);
    }
}

void DialogSeznamPruvodek::on_seznamDnesVytistene_activated(const QModelIndex &index) {
    Q_UNUSED(index)
    on_tlacitkoZaraditDoFronty_clicked();
}

void DialogSeznamPruvodek::on_tlacitkoZaraditDoFronty_clicked() {
    if (ui->seznamPruvodkyVeFronte->styleSheet() == "color: grey;") {
        ui->seznamPruvodkyVeFronte->clear();
        ui->seznamPruvodkyVeFronte->setStyleSheet("");
    }
    ui->seznamPruvodkyVeFronte->addItem(ui->seznamDnesVytistene->item(aktivniRadek_)->text());
    obnovenePruvodky_.append(p_dnesVytistenePruvodky_->at(aktivniRadek_));
    provedeneZmeny_ = true;
}

void DialogSeznamPruvodek::on_tlacitkoZrusit_clicked() {
    bool pokracovat = (provedeneZmeny_ ? OknoZprav::potvrzeni(OknoZprav::Otazka,
                                                              "Provedené změny v tiskové frontě nebudou uloženy. "
                                                              "Chcete pokračovat?")
                                       : true);

    if (pokracovat) {
        reject();
    }
}

void DialogSeznamPruvodek::on_tlacitkoPotvrdit_clicked() {
    p_nevytistenePruvodky_->append(obnovenePruvodky_);

    accept();
}

DialogSeznamPruvodek::~DialogSeznamPruvodek() {
    delete ui;
}

void DialogSeznamPruvodek::keyPressEvent(QKeyEvent *udalost) {
    // Ctrl+Enter potvrdi zmeny
    if ((udalost->key() == Qt::Key_Enter || udalost->key() == Qt::Key_Return)
            && (udalost->modifiers().testFlag(Qt::ControlModifier))) {
        ui->tlacitkoPotvrdit->click();
    }
    else {
        QDialog::keyPressEvent(udalost);
    }
}

bool DialogSeznamPruvodek::naplnitSeznamPruvodek(QList<Zavoz> *seznamPruvodek, QListWidget *seznam) {
    if (!seznamPruvodek->isEmpty()) {
        seznam->clear();
        seznam->setStyleSheet("");
        foreach (Zavoz zavoz, *seznamPruvodek) {
            seznam->addItem(QString(zavoz.celeJmeno() + " (" + zavoz.ulice() + ", " + zavoz.obec() + ")"));
        }
        return true;
    }

    seznam->item(0)->setTextAlignment(Qt::AlignCenter);
    return false;
}
