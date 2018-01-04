#include "AbstraktniDialogPrehled.h"
#include "ui_AbstraktniDialogPrehled.h"

AbstraktniDialogPrehled::AbstraktniDialogPrehled(QWidget *rodic) :
    QDialog(rodic), naseptavac_(new Naseptavac),
    ui(new Ui::AbstraktniDialogPrehled) {
    ui->setupUi(this);
    validator_ = new ValidatorVstupu(ui->popisekUpozorneni);

    ui->vstupDatumOd->setDate(datumOd_);
    ui->vstupDatumOd->calendarWidget()->setFirstDayOfWeek(Qt::Monday);

    ui->vstupDatumDo->setDate(datumDo_);
    ui->vstupDatumDo->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
}

AbstraktniDialogPrehled::~AbstraktniDialogPrehled() {
    delete ui;
    delete validator_;
    delete naseptavac_;
}

void AbstraktniDialogPrehled::on_vstupDatumOd_dateChanged(const QDate &datum) {
    datumOd_ = datum;
    resetovatComboBoxObdobi();
    zpristupnitTabulku(false);
}

void AbstraktniDialogPrehled::on_vstupDatumDo_dateChanged(const QDate &datum) {
    datumDo_ = datum;
    resetovatComboBoxObdobi();
    zpristupnitTabulku(false);
}

void AbstraktniDialogPrehled::on_comboBoxObdobi_currentIndexChanged(int index) {
    switch (index) {
        case DenNejblizsihoZavozu:
            datumDo_ = datumOd_ = Datum::denNejblizsihoZavozu();
            break;
        case PredchoziMesic:
            datumOd_ = Datum::prvniDenPredchozihoMesice();
            datumDo_ = Datum::posledniDenPredchozihoMesice();
            break;
        case PredchoziKvartal:
            datumOd_ = Datum::prvniDenPredchozihoKvartalu();
            datumDo_ = Datum::posledniDenPredchozihoKvartalu();
            break;
        case PredchoziRok:
            datumOd_ = Datum::prvniDenPrechodzihoRoku();
            datumDo_ = Datum::posledniDenPredchozihoRoku();
            break;
        case CelaDoba:
            datumOd_ = zacatek_;
            datumDo_ = dnes_.addDays(7);
    }

    ui->vstupDatumOd->setDate(datumOd_);
    ui->vstupDatumDo->setDate(datumDo_);

    zpristupnitTabulku(false);
}

void AbstraktniDialogPrehled::on_tlacitkoVyhledat_clicked() {
    if (casoveRozpetiValidni()) {
        zobrazitZaznamy();
    }
    else {
        validator_->zobrazitUpozorneni(Text::kurziva("Datum od") + " nesmí být větší než " + Text::kurziva("datum do"));
    }
}

//void AbstraktniDialogPrehled::on_tabulka_doubleClicked(const QModelIndex &index) {
//    if (typ_ == Pacienti) {
//        Pacient *pacient;
//        Databaze::vyhledatPacienta(ui->tabulka->model()->index(index.row(), 1).data().toString(),
//                                   ui->tabulka->model()->index(index.row(), 2).data().toString(),
//                                   pacient);
//        if (DialogPacient::otevritUpravaPacienta(pacient, this)) {
//            provedenyZmenyVDb_ = true;
//            queryModel_->query().exec();
//        }
//    }
//}

void AbstraktniDialogPrehled::on_tlacitkoExportovatCsv_clicked() {
    QString nazevSouboru = ziskatNazevCsvSouboru();

    if (!nazevSouboru.isEmpty()) {
        QFile csvSoubor(nazevSouboru);
        if (csvSoubor.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream vystup(&csvSoubor);
            vystup << vytvoritTextovaDataZTabulky();

            csvSoubor.close();
        }
        else {
            OknoZprav::upozorneni("Soubor se nepodařilo otevřít:", csvSoubor.errorString());
        }
    }
}

void AbstraktniDialogPrehled::on_tlacitkoZavrit_clicked() {
    close();
}

void AbstraktniDialogPrehled::keyPressEvent(QKeyEvent *udalost) {
    // Ctrl+Enter vyhleda zaznamy
    if ((udalost->key() == Qt::Key_Enter || udalost->key() == Qt::Key_Return)
            && (udalost->modifiers().testFlag(Qt::ControlModifier))) {
        ui->tlacitkoVyhledat->click();
    }
    // Ctrl+E spusti export CSV
    else if (udalost->key() == Qt::Key_E && udalost->modifiers().testFlag(Qt::ControlModifier)) {
        ui->tlacitkoExportovatCsv->click();
    }
    else {
        QDialog::keyPressEvent(udalost);
    }
}

void AbstraktniDialogPrehled::resetovatComboBoxObdobi() {
    bool resetovat;
    switch (ui->comboBoxObdobi->currentIndex()) {
        case DenNejblizsihoZavozu:
            resetovat = (datumOd_ != Datum::denNejblizsihoZavozu() || datumDo_ != Datum::denNejblizsihoZavozu());
            break;
        case PredchoziMesic:
            resetovat = (datumOd_ != Datum::prvniDenPredchozihoMesice() || datumDo_ != Datum::posledniDenPredchozihoMesice());
            break;
        case PredchoziKvartal:
            resetovat = (datumOd_ != Datum::prvniDenPredchozihoKvartalu() || datumDo_ != Datum::posledniDenPredchozihoKvartalu());
            break;
        case PredchoziRok:
            resetovat = (datumOd_ != Datum::prvniDenPrechodzihoRoku() || datumDo_ != Datum::posledniDenPredchozihoRoku());
            break;
        case CelaDoba:
            resetovat = (datumOd_ != zacatek_ || datumDo_ != dnes_.addDays(7));
            break;
        default:
            resetovat = false;
    }

    if (resetovat) {
        ui->comboBoxObdobi->setCurrentIndex(LibovolneObodobi);
    }
}

bool AbstraktniDialogPrehled::casoveRozpetiValidni() {
    return (datumOd_ <= datumDo_);
}

void AbstraktniDialogPrehled::povolitVyhledavaciTlacitko(bool povolit)
{
    ui->tlacitkoVyhledat->setEnabled(povolit);
}

void AbstraktniDialogPrehled::zobrazitZaznamy() {
    if (vyhledatZaznamy()) {
        filterModel_->setDynamicSortFilter(true);
        filterModel_->setSourceModel(queryModel_);

        ui->tabulka->setModel(filterModel_);
        ui->tabulka->resizeColumnsToContents();
        ui->tabulka->horizontalHeader()->setStretchLastSection(true);
        ui->tabulka->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tabulka->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tabulka->setSortingEnabled(true);
        ui->tabulka->sortByColumn(0, Qt::AscendingOrder);

        zpristupnitTabulku(true);

        validator_->smazatUpozorneni();
    }
    else {
        zpristupnitTabulku(false);
        vyprazdnitTabulku();
        validator_->zobrazitUpozorneni("Pro zadané období nebyly nalezeny žádné záznamy");
    }
}

void AbstraktniDialogPrehled::zpristupnitTabulku(bool zpristupnit) {
    ui->tabulka->setEnabled(zpristupnit);
    ui->tlacitkoExportovatCsv->setEnabled(zpristupnit);
    ui->popisekUpozorneni->clear();
}

void AbstraktniDialogPrehled::vyprazdnitTabulku() {
    ui->tabulka->setModel(nullptr);
}

QString AbstraktniDialogPrehled::ziskatNazevCsvSouboru() {
    return QFileDialog::getSaveFileName(this, "Export CSV",
                                        QString(QDir::currentPath() + "/export/" + navrhnoutNazevCsvSouboru()),
                                        "Soubory CSV (*.csv)");
}

QString AbstraktniDialogPrehled::vytvoritTextovaDataZTabulky() {
    QString textovaData;
    int radky = filterModel_->rowCount();
    int sloupce = filterModel_->columnCount();

    for (int i = 0; i < sloupce; i++) {
        textovaData += "\"";
        textovaData += filterModel_->headerData(i, Qt::Horizontal).toString();
        textovaData += "\", ";
    }

    textovaData += "\n";

    for (int i = 0; i < radky; i++) {
        for (int j = 0; j < sloupce; j++) {
            textovaData +="\"";
            textovaData += filterModel_->data(filterModel_->index(i,j)).toString();
            textovaData += "\", ";
        }
        textovaData += "\n";
    }

    return textovaData;
}
