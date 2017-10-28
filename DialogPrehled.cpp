#include "DialogPrehled.h"
#include "ui_DialogPrehled.h"

DialogPrehled::DialogPrehled(TypPrehledu typ, QWidget *rodic) :
    QDialog(rodic), typ_(typ), naseptavac_(new Naseptavac),
    ui(new Ui::DialogPrehled)
{
    ui->setupUi(this);

    validator_ = new ValidatorVstupu(ui->popisekUpozorneni);
    validator_->nastavitFiltrUdalosti(QList<QLineEdit*>({ui->vstupPrijmeni, ui->vstupJmeno}));

    ui->sekcePacient->setVisible(false);
    ui->sekcePomucka->setVisible(false);
    ui->sekceUzivatel->setVisible(false);

    ui->vstupDatumOd->setDate(datumOd_);
    ui->vstupDatumOd->calendarWidget()->setFirstDayOfWeek(Qt::Monday);

    ui->vstupDatumDo->setDate(datumDo_);
    ui->vstupDatumDo->calendarWidget()->setFirstDayOfWeek(Qt::Monday);

    switch (typ_) {
    case ZavozyPacienta:
        this->setWindowTitle("Přehled závozů pro pacienta");
        ui->vstupPrijmeni->setCompleter(naseptavac_->naseptavacPrijmeni());
        ui->sekcePacient->setVisible(true);
        ui->tlacitkoJinyPacient->setVisible(false);
        break;
    case PomuckaZaObdobi:
        this->setWindowTitle("Přehled závozů jednotlivé pomůcky");
        ui->vstupKodVzp->setValidator(new QIntValidator(0, 9999999, this));
        ui->sekcePomucka->setVisible(true);
        break;
    case ZavozyUzivatele:
        this->setWindowTitle("Přehled závozů vychystaných uživatelem");
        ui->sekceUzivatel->setVisible(true);

        foreach (QString uzivatel, Databaze::seznamUzivatelu(false)) {
            ui->comboBoxUzivatel->addItem(uzivatel);
        }

        break;
    case ZavozyZaObdobi:
        this->setWindowTitle("Přehled závozů za období");

        povolitVyhledavaciTlacitko(true);
        ui->sekcePomucka->setVisible(false);
        ui->sekcePacient->setVisible(false);
        break;
    case Pacienti:
        this->setWindowTitle("Přehled pacientů vedených v databázi");

        ui->sekceCasoveRozmezi->setVisible(false);
        ui->popisekUpozorneni->setVisible(false);

        zobrazitZaznamy();
        break;
    }
}

DialogPrehled::DialogPrehled(Pacient *pacient, QWidget *rodic)
    : DialogPrehled(ZavozyPacienta, rodic)
{
    ui->vstupPrijmeni->setText(pacient->prijmeni());
    ui->vstupJmeno->setText(pacient->jmeno());

    // vyhleda pacienta a znemozni vyhledani jineho
    on_tlacitkoNajitPacienta_clicked();
    ui->vstupJmeno->setEnabled(true);
    ui->tlacitkoJinyPacient->setVisible(false);

    // rovnou vyhleda a zobrazi zaznamy za celou dobu
    ui->comboBoxObdobi->setCurrentIndex(CelaDoba);
    zobrazitZaznamy();
}

DialogPrehled::~DialogPrehled() {
    delete ui;
    delete validator_;
    delete naseptavac_;
}

bool DialogPrehled::otevrit(DialogPrehled::TypPrehledu typ) {
    provedenyZmenyVDb_ = false; // reset na vychozi hodnotu
    DialogPrehled *dialog = new DialogPrehled(typ);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->showMaximized();
    dialog->exec();
    return provedenyZmenyVDb_;  // bude false, dokud v typu Pacienti nedoslo k uprave pacienta
}

bool DialogPrehled::otevritZavozyAktualnihoPacienta(Pacient *pacient) {
    DialogPrehled *dialog = new DialogPrehled(pacient);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->showMaximized();
    return dialog->exec();
}

void DialogPrehled::on_vstupPrijmeni_textChanged(const QString &text) {
    ui->tlacitkoNajitPacienta->setEnabled(!text.isEmpty() && !ui->vstupJmeno->text().isEmpty());
    ui->vstupJmeno->setEnabled(false);
}

void DialogPrehled::on_vstupPrijmeni_returnPressed() {
    ui->vstupJmeno->setEnabled(true);
    ui->vstupJmeno->setCompleter(naseptavac_->naseptavacJmen(ui->vstupPrijmeni->text()));
    ui->vstupJmeno->clear();
    ui->vstupJmeno->setFocus();
}

void DialogPrehled::on_vstupJmeno_textChanged(const QString &text) {
    ui->tlacitkoNajitPacienta->setEnabled(!text.isEmpty() && !ui->vstupPrijmeni->text().isEmpty());
}

void DialogPrehled::on_vstupJmeno_returnPressed() {
    on_tlacitkoNajitPacienta_clicked();
}

void DialogPrehled::on_tlacitkoNajitPacienta_clicked() {
    pacId_ = Databaze::ziskatIdPacienta(ui->vstupPrijmeni->text(), ui->vstupJmeno->text());

    povolitVyhledavaciTlacitko(pacId_);
    zpristupnitVstupy(!pacId_);
    ui->tlacitkoJinyPacient->setEnabled(pacId_);

    if (!pacId_) {
        validator_->zobrazitUpozorneni(QString("Pacient " + Text::kurziva(ui->vstupPrijmeni->text(), ui->vstupJmeno->text())
                                               + " nebyl nalezen v databázi"));
    }
}

void DialogPrehled::on_tlacitkoJinyPacient_clicked() {
    zpristupnitVstupy(true);
    ui->vstupPrijmeni->clear();
    ui->vstupJmeno->clear();
    ui->vstupPrijmeni->setFocus();

    zpristupnitTabulku(false);
}

void DialogPrehled::on_vstupKodVzp_textChanged(const QString &text) {
    ui->tlacitkoNajitPomucku->setEnabled(!text.isEmpty());
    ui->tlacitkoNajitPomucku->setVisible(true);
    ui->popisekPomucka->clear();
    zpristupnitTabulku(false);
    povolitVyhledavaciTlacitko(false);
}

void DialogPrehled::on_vstupKodVzp_returnPressed() {
    on_tlacitkoNajitPomucku_clicked();
}

void DialogPrehled::on_tlacitkoNajitPomucku_clicked() {
    QString nazev, doplnek;

    if (Databaze::vyhledatPomucku(ui->vstupKodVzp->text(), nazev, doplnek)
        && (ui->vstupKodVzp->text().toInt() != 0)) {
        QString zobrazenyNazev = nazev;
        if (!doplnek.isEmpty())
            zobrazenyNazev += QString("<br>" + doplnek);
        ui->popisekPomucka->setText(zobrazenyNazev);
        ui->tlacitkoNajitPomucku->setVisible(false);
        povolitVyhledavaciTlacitko(true);
    }
    else {
        validator_->zobrazitUpozorneni(QString("Číselník neobsahuje položku s kódem " + ui->vstupKodVzp->text()));
    }

    ui->vstupKodVzp->selectAll();
}

void DialogPrehled::on_comboBoxUzivatel_currentIndexChanged(int index) {
    if (index) {
        QString uzivJmeno = ui->comboBoxUzivatel->currentText().section(" ", 0, 0);
        QString celeJmeno;
        bool admin;

        Databaze::nacistUdajeUzivatele(uzivJmeno, celeJmeno, admin);

        ui->popisekUzivatel->setText(celeJmeno);
    }
    else {
        ui->popisekUzivatel->clear();
    }

    povolitVyhledavaciTlacitko(index);
    validator_->smazatUpozorneni();
    zpristupnitTabulku(false);
}

void DialogPrehled::on_vstupDatumOd_dateChanged(const QDate &datum) {
    datumOd_ = datum;
    resetovatComboBoxObdobi();
    zpristupnitTabulku(false);
}

void DialogPrehled::on_vstupDatumDo_dateChanged(const QDate &datum) {
    datumDo_ = datum;
    resetovatComboBoxObdobi();
    zpristupnitTabulku(false);
}

void DialogPrehled::on_comboBoxObdobi_currentIndexChanged(int index) {
    switch (index) {
    case PredchoziMesic:
        datumOd_ = prvniDenPredchozihoMesice();
        datumDo_ = posledniDenPredchozihoMesice();
        break;
    case PredchoziKvartal:
        datumOd_ = prvniDenPredchozihoKvartalu();
        datumDo_ = posledniDenPredchozihoKvartalu();
        break;
    case PredchoziRok:
        datumOd_ = prvniDenPrechodzihoRoku();
        datumDo_ = posledniDenPredchozihoRoku();
        break;
    case CelaDoba:
        datumOd_ = zacatek_;
        datumDo_ = dnes_.addDays(7);
    }

    ui->vstupDatumOd->setDate(datumOd_);
    ui->vstupDatumDo->setDate(datumDo_);

    zpristupnitTabulku(false);
}

void DialogPrehled::on_tlacitkoVyhledat_clicked() {
    if (casoveRozpetiValidni()) {
        zobrazitZaznamy();
    }
    else {
        validator_->zobrazitUpozorneni(Text::kurziva("Datum od") + " nesmí být větší než " + Text::kurziva("datum do"));
    }
}

void DialogPrehled::on_tabulka_doubleClicked(const QModelIndex &index) {
    if (typ_ == Pacienti) {
        Pacient *pacient;
        Databaze::vyhledatPacienta(ui->tabulka->model()->index(index.row(), 1).data().toString(),
                                   ui->tabulka->model()->index(index.row(), 2).data().toString(),
                                   pacient);
        if (DialogPacient::otevritUpravaPacienta(pacient)) {
            provedenyZmenyVDb_ = true;
            queryModel_->query().exec();
        }
    }
}

void DialogPrehled::on_tlacitkoExportovatCsv_clicked() {
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

void DialogPrehled::on_tlacitkoZavrit_clicked() {
    close();
}

void DialogPrehled::keyPressEvent(QKeyEvent *udalost) {
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

void DialogPrehled::zpristupnitVstupy(bool zpristupnit) {
    if (typ_ == ZavozyPacienta) {
        foreach (QLineEdit *policko, QList<QLineEdit*>({ui->vstupPrijmeni, ui->vstupJmeno})) {
            policko->setReadOnly(!zpristupnit);
            policko->setFrame(zpristupnit);
            policko->setStyleSheet(zpristupnit ? ""
                                               : "background-color: transparent; font-weight: bold");
            zpristupnit ? policko->installEventFilter(validator_)
                        : policko->removeEventFilter(validator_);
        }

        ui->tlacitkoNajitPacienta->setVisible(zpristupnit);
        ui->tlacitkoJinyPacient->setVisible(!zpristupnit);
    }
}

void DialogPrehled::resetovatComboBoxObdobi() {
    bool resetovat;
    switch (ui->comboBoxObdobi->currentIndex()) {
    case PredchoziMesic: resetovat = (datumOd_ != prvniDenPredchozihoMesice() || datumDo_ != posledniDenPredchozihoMesice());
        break;
    case PredchoziKvartal: resetovat = (datumOd_ != prvniDenPredchozihoKvartalu() || datumDo_ != posledniDenPredchozihoKvartalu());
        break;
    case PredchoziRok: resetovat = (datumOd_ != prvniDenPrechodzihoRoku() || datumDo_ != posledniDenPredchozihoRoku());
        break;
    case CelaDoba: resetovat = (datumOd_ != zacatek_ || datumDo_ != dnes_.addDays(7));
        break;
    default:
        resetovat = false;
    }

    if (resetovat)
        ui->comboBoxObdobi->setCurrentIndex(LibovolneObodobi);
}

bool DialogPrehled::casoveRozpetiValidni() {
    return (datumOd_ <= datumDo_);
}

void DialogPrehled::povolitVyhledavaciTlacitko(bool povolit) {
    ui->tlacitkoVyhledat->setEnabled(povolit);
}

void DialogPrehled::zobrazitZaznamy() {
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

bool DialogPrehled::vyhledatZaznamy() {
    switch (typ_) {
    case ZavozyPacienta:
        return Databaze::prehledZavozuProPacienta(pacId_, datumOd_, datumDo_, queryModel_);

    case ZavozyUzivatele:
        return Databaze::prehledZavozuUzivatele(ui->comboBoxUzivatel->currentText().section(" ", 0, 0),
                                                datumOd_, datumDo_, queryModel_);

    case PomuckaZaObdobi:
        return Databaze::prehledZavozuPomucky(ui->vstupKodVzp->text(), datumOd_, datumDo_, queryModel_);

    case ZavozyZaObdobi:
        return Databaze::prehledZavozu(datumOd_, datumDo_, queryModel_);

    case Pacienti:
        ui->tabulka->setToolTip("Dvojklikem na řádek v tabulce otevřete dialog úpravy příslušného pacienta");
        return Databaze::prehledPacientu(queryModel_);

    default:
        return false;
    }
}

void DialogPrehled::zpristupnitTabulku(bool zpristupnit) {
    ui->tabulka->setEnabled(zpristupnit);
    ui->tlacitkoExportovatCsv->setEnabled(zpristupnit);
    ui->popisekUpozorneni->clear();
}

void DialogPrehled::vyprazdnitTabulku() {
    ui->tabulka->setModel(nullptr);
}

QDate DialogPrehled::prvniDenPredchozihoMesice() {
    int rok, mesic;

    if (dnes_.month() > 1) {
        rok = dnes_.year();
        mesic = dnes_.month() - 1;
    }
    else {
        rok = dnes_.year() - 1;
        mesic = 12;
    }

    return QDate(rok, mesic, 1);
}

QDate DialogPrehled::posledniDenPredchozihoMesice() {
    int rok, mesic;

    if (dnes_.month() > 1) {
        rok = dnes_.year();
        mesic = dnes_.month() - 1;
    }
    else {
        rok = dnes_.year() - 1;
        mesic = 12;
    }

    QDate datum = QDate(rok, mesic, 1);
    datum.setDate(rok, mesic, datum.daysInMonth());
    return datum;
}

QDate DialogPrehled::prvniDenPredchozihoKvartalu() {
    int rok, mesic;

    switch (dnes_.month()) {
    case 1:
    case 2:
    case 3:
        rok = dnes_.year() - 1;
        mesic = 10;
        break;
    case 4:
    case 5:
    case 6:
        rok = dnes_.year();
        mesic = 1;
        break;
    case 7:
    case 8:
    case 9:
        rok = dnes_.year();
        mesic = 4;
        break;
    case 10:
    case 11:
    case 12:
    default:
        rok = dnes_.year();
        mesic = 7;
    }

    return QDate(rok, mesic, 1);
}

QDate DialogPrehled::posledniDenPredchozihoKvartalu() {
    int rok, mesic;

    switch (dnes_.month()) {
    case 1:
    case 2:
    case 3:
        rok = dnes_.year() - 1;
        mesic = 12;
        break;
    case 4:
    case 5:
    case 6:
        rok = dnes_.year();
        mesic = 9;
        break;
    case 7:
    case 8:
    case 9:
        rok = dnes_.year();
        mesic = 6;
        break;
    case 10:
    case 11:
    case 12:
    default:
        rok = dnes_.year();
        mesic = 3;
    }

    QDate datum = QDate(rok, mesic, 1);
    datum.setDate(rok, mesic, datum.daysInMonth());
    return datum;
}

QDate DialogPrehled::prvniDenPrechodzihoRoku() {
    return QDate(dnes_.year() - 1, 1, 1);
}

QDate DialogPrehled::posledniDenPredchozihoRoku() {
    return QDate(dnes_.year() - 1, 12, 31);
}

QString DialogPrehled::ziskatNazevCsvSouboru() {
    QString navrhNazvuSouboru;
    switch (typ_) {
    case ZavozyPacienta:
        navrhNazvuSouboru += QString(ui->vstupPrijmeni->text() + " " + ui->vstupJmeno->text().at(0) + ". ");
        break;
    case ZavozyUzivatele:
        navrhNazvuSouboru += QString(ui->comboBoxUzivatel->currentText().section(" ", 0, 0) + " - závozy ");
        break;
    case PomuckaZaObdobi:
        navrhNazvuSouboru += QString("ZP" + ui->vstupKodVzp->text() + " ");
        break;
    case ZavozyZaObdobi:
        navrhNazvuSouboru += "závozy ";
        break;
    default:
        break;
    }

    if (typ_ != Pacienti)
        navrhNazvuSouboru += QString(datumOd_.toString(Qt::ISODate) + " - " + datumDo_.toString(Qt::ISODate) + ".csv");
    else
        navrhNazvuSouboru = QString("Seznam pacientů k " + QDate::currentDate().toString(Qt::ISODate));

    return QFileDialog::getSaveFileName(this, "Export CSV",
                                        QString(QDir::currentPath() + "/export/" + navrhNazvuSouboru),
                                        "Soubory CSV (*.csv)");
}

QString DialogPrehled::vytvoritTextovaDataZTabulky() {
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

bool DialogPrehled::provedenyZmenyVDb_ = false;
