#include "DialogPrehledPacientu.h"
#include "ui_AbstraktniDialogPrehled.h"

DialogPrehledPacientu::DialogPrehledPacientu(QWidget *rodic)
    : AbstraktniDialogPrehled(rodic)
{
    // prizbusobi texty
    this->setWindowTitle("Přehled pacientů vedených v databázi");

    // nastavi widgety
    ui->sekceVyhledavaciKriterium->setVisible(false);
    ui->sekceCasoveRozmezi->setVisible(false);
    ui->popisekUpozorneni->setVisible(false);

    ui->tabulka->setToolTip("Dvojklikem na řádek v tabulce otevřete dialog úpravy příslušného pacienta");
    zobrazitZaznamy();

    // navaze udalosti
    connect(ui->tabulka, &QTableView::doubleClicked,
            this, &DialogPrehledPacientu::upravitZvolenehoPacienta);
}

bool DialogPrehledPacientu::otevrit(QWidget *rodic) {
    provedenyZmenyVDb_ = false; // reset na vychozi hodnotu
    DialogPrehledPacientu *dialog = new DialogPrehledPacientu(rodic);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->showMaximized();
    dialog->exec();
    return provedenyZmenyVDb_;  // bude false, dokud v typu Pacienti nedoslo k uprave pacienta
}

void DialogPrehledPacientu::upravitZvolenehoPacienta(const QModelIndex &index) {
    Pacient *pacient;
    Databaze::vyhledatPacienta(ui->tabulka->model()->index(index.row(), 1).data().toString(),
                               ui->tabulka->model()->index(index.row(), 2).data().toString(),
                               pacient);
    if (DialogPacient::otevritUpravaPacienta(pacient, this)) {
        provedenyZmenyVDb_ = true;
        queryModel_->query().exec();
    }
}

bool DialogPrehledPacientu::vyhledatZaznamy() {
    return Databaze::prehledPacientu(queryModel_);
}

QString DialogPrehledPacientu::navrhnoutNazevCsvSouboru() {
    return QString("Seznam pacientů k " + QDate::currentDate().toString(Qt::ISODate));
}

bool DialogPrehledPacientu::provedenyZmenyVDb_ = false;
