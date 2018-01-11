#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      naseptavac_(new Naseptavac),
      ui(new Ui::MainWindow) {
    ui->setupUi(this);

    vstupyPrijmeniAJmeno_ << ui->vstupPrijmeni << ui->vstupJmeno;

    validatorPacient_ = new ValidatorVstupu(ui->popisekPacientUpozorneni);
    validatorPacient_->nastavitFiltrUdalosti(vstupyPrijmeniAJmeno_);

    validatorPomucky_ = new ValidatorVstupu(ui->popisekPomuckyUpozorneni);
    validatorPomucky_->nastavitFiltrUdalosti(ui->vstupNovaPolozka);

    // nastavit velikost a polohu okna
    QTimer::singleShot(90, this, &MainWindow::vycentrovatOkno);

    // nastavit titulek
    this->setWindowTitle(QString("Závozy v" + cisloVerze));

    if (Databaze::prihlasitKDatabazi(Soubory::databaze)) {
        // stavovy radek
        stavovyNapis_ = new QLabel(this);
        indikator_ = new QProgressBar(this);
        indikator_->setRange(0, 100);
        indikator_->hide();

        statusBar()->addWidget(stavovyNapis_);
        statusBar()->addWidget(indikator_);

        // pripravit naseptavac prijmeni pacientu
        pripravitNaseptavacPrijmeni();

        // pripravit mrizku pomucek
        mrizkaPomucek_ = new WidgetSubsekcePomuckySeznam(this);

        ui->rolovatelnaOblastSeznamPomucek->setWidget(mrizkaPomucek_);

        // pripravit vstup Nova polozka
        ui->vstupNovaPolozka->setValidator(new QIntValidator(0, 9999999, this));
        ui->vstupNovaPolozka->installEventFilter(new FiltrUdalostiVstupNovaPolozka(this));

        // pripravit vzstup Datum zavozu
        pripravitVstupDatumZavozu();

        // nacist pripadne nevytistene pruvodky
        if (Soubory::existujiNevytistene()) {
            QTimer::singleShot(150, this, &MainWindow::nacistNevytistenePruvodky);
        }

        // nacist pripadne dnes vytistene pruvodky k dotisku
        if (Soubory::existujiDnesVytistene()) {
            QTimer::singleShot(150, this, &MainWindow::nacistDnesVytistenePruvodky);
        }

        // jestli jsou uzivatele v databazi, vyzvat k prihlaseni uzivatele
        if (Databaze::pocetAktivnichUzivatelu() > 0) {
            QTimer::singleShot(100, this, &MainWindow::on_actionPrihlasit_triggered);
        }
        // jinak vyzvat k zalozeni prvniho uzivatele
        else {
            QTimer::singleShot(100, this, &MainWindow::vyzvatKZalozeniPrvnihoUzivatele);
        }
    }
    else {  // databaze neotevrena - aplikace se ukonci
        OknoZprav::kritickaChyba(QString("Soubor databáze " + Text::kurziva(Soubory::databaze)
                                         + " chybí nebo je poškozen."),
                                 "Aplikaci není možné používat, proto bude ukončena.");
        ukoncitAplikaciSChybou();
    }
}

MainWindow::~MainWindow() {
    delete ui;
    delete naseptavac_;
    delete validatorPacient_;
    delete validatorPomucky_;
}

void MainWindow::on_actionPrihlasit_triggered() {
    prihlasenyUzivatel_ = new Uzivatel("Bed", "", "", 1);
    povolitUzivatelskeAkce(true);
    povolitAdministratorskeAkce(true);

//    if (DialogPrihlaseni::otevrit(prihlasenyUzivatel_, this)) {
//        povolitUzivatelskeAkce(true);
//        povolitAdministratorskeAkce(prihlasenyUzivatel_->admin());
//        this->setWindowTitle(QString("Závozy " + cisloVerze + " - pracuje " + prihlasenyUzivatel_->celeJmeno()));
//        zobrazitVerziCiselniku();
//    }
}

void MainWindow::on_actionZmenitHeslo_triggered() {
    if (DialogUpravitSmazatUzivatele::otevritZmenaHesla(prihlasenyUzivatel_, this))
        nastavitZmenuDatabaze(true);
}

void MainWindow::on_actionPridatUzivatele_triggered() {
    bool zadniUzivateleVDatabazi = (Databaze::pocetAktivnichUzivatelu() == 0);
    bool zalozenUzivatel = DialogNovyUzivatel::otevrit(this);

    if (zalozenUzivatel)
        nastavitZmenuDatabaze(true);

    // po vynucenem zalozeni prvniho uzivatele vyzve uzivatele k prihlaseni
    if (zadniUzivateleVDatabazi && zalozenUzivatel) {
        vyzvatKPrihlaseniPrvnihoUzivatele();
    }
}

void MainWindow::on_actionUpravitSmazatUzivatele_triggered() {
    if (DialogUpravitSmazatUzivatele::otevritUpravaUzivatele(prihlasenyUzivatel_, this))
        nastavitZmenuDatabaze(true);
}

void MainWindow::on_actionOdhlasit_triggered() {
    if (schvalitZtratuNeulozenychZmen()) {
        delete prihlasenyUzivatel_;
        prihlasenyUzivatel_ = nullptr;
        this->setWindowTitle("Závozy " + cisloVerze);
        povolitUzivatelskeAkce(false);
        povolitAdministratorskeAkce(false);
        zobrazitUdajeAktualnihoPacienta(false);
        nastavitNeulozeneZmeny(false);
        on_actionPrihlasit_triggered();
    }
}

void MainWindow::on_actionUkoncitAplikaci_triggered() {
    close();
}

void MainWindow::on_actionNovyPacient_triggered() {
    if (DialogPacient::otevritNovyPacient(this)) {
        nastavitZmenuDatabaze(true);
        pripravitNaseptavacPrijmeni();  // aktualizuje naseptavac
    }
}

void MainWindow::on_actionUpravitPacienta_triggered() {
    if (DialogPacient::otevritUpravaPacienta(this)) {
        nastavitZmenuDatabaze(true);
        pripravitNaseptavacPrijmeni();  // aktualizuje naseptavac
    }
}

void MainWindow::on_actionUpravitAktualnihoPacienta_triggered() {
    if (DialogPacient::otevritUpravaPacienta(aktualniPacient_, this)) {
        zobrazitUdajeAktualnihoPacienta(true);  // aktualizace policek po uprave
        nastavitZmenuDatabaze(true);
    }
}

void MainWindow::on_actionZrusitVolbuPacienta_triggered() {
    if (schvalitZtratuNeulozenychZmen()) {
        delete aktualniPacient_;

        zobrazitUdajeAktualnihoPacienta(false);
        nastavitNeulozeneZmeny(false);
        ui->vstupPrijmeni->setFocus();
    }
}

void MainWindow::on_actionZobrazitSeznamVsechPacientu_triggered() {
    if (DialogPrehledPacientu::otevrit(this)) {
        nastavitZmenuDatabaze(true);    // v pripade, ze uzivatel zmenil udaje pacienta pres tabulku
        pripravitNaseptavacPrijmeni();  // aktualizuje naseptavac
    }
}

void MainWindow::on_actionVytisknoutNevytistenePruvodky_triggered() {
    on_tlacitkoVytisknoutPruvodky_clicked();
}

void MainWindow::on_actionZobrazitPrehledNevytistenychPruvodek_triggered() {
    DialogSeznamPruvodek::otevritNevytistene(&nevytistene_, this);
}

void MainWindow::on_actionOpetovneZaraditPruvodkyDoTiskoveFronty_triggered() {
    if (DialogSeznamPruvodek::otevritOpetovneZarazeniDoFronty(&dnesVytistene_, &nevytistene_, this)) {
        ulozitSeznamPruvodekDoSouboru(nevytistene_, Soubory::nevytistene);
        zobrazitPocetNevytistenychPruvodek();
    }
}

void MainWindow::on_actionPrehledZavozuProPacienta_triggered() {
    DialogPrehledZavozuProPacienta::otevrit(this);
}

void MainWindow::on_actionPrehledZavozuProAktualnihoPacienta_triggered() {
    DialogPrehledZavozuProPacienta::otevritZavozyAktualnihoPacienta(aktualniPacient_, this);
}

void MainWindow::on_actionPrehledZavozuVychystanychUzivatelem_triggered() {
    DialogPrehledZavozuVychystanychUzivatelem::otevrit(this);
}

void MainWindow::on_actionPrehledZavozuJednotlivePomucky_triggered() {
    DialogPrehledZavozuJednotlivePomucky::otevrit(this);
}

void MainWindow::on_actionPrehledVsechZavozuZaObdobi_triggered() {
    DialogPrehledVsechZavozuZaObdobi::otevrit(this);
}

void MainWindow::on_actionOtevritWebstrankuSCiselnikyPomucek_triggered() {
    QDesktopServices::openUrl(QUrl(Soubory::ciselnikWeb));
}

void MainWindow::on_actionAktualizovatCiselnikPomucekZeSouboru_triggered() {
    QString souborCiselniku = QFileDialog::getOpenFileName(this, "Otevřít soubor s číselníkem",
                                                           QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                           "Textové soubory (*.txt)");

    if (!souborCiselniku.isEmpty()) {
        AktualizatorCiselniku *aktualizator = new AktualizatorCiselniku;

        connect(aktualizator, &AktualizatorCiselniku::aktualizaceSpustena,
                this, &MainWindow::zobrazitPrubehAktualizace);
        connect(aktualizator, &AktualizatorCiselniku::zatimZpracovano,
                indikator_, &QProgressBar::setValue);
        connect(aktualizator, &AktualizatorCiselniku::aktualizaceDokoncena,
                this, &MainWindow::dokoncitProcesAktualizace);

        aktualizator->aktualizovatCiselnik(souborCiselniku, prihlasenyUzivatel_->uzivJmeno());

        nastavitZmenuDatabaze(true);

        delete aktualizator;
    }
}

void MainWindow::on_actionSpustitOkamzitouZalohuDatabaze_triggered() {
    QString verzeDb;
    if (Soubory::zalohovatDatabazi(&verzeDb)) {
        statusBar()->showMessage(QString("Byla vytvořena záloha databáze č. " + verzeDb), 5000);
        nastavitZmenuDatabaze(false);
    }
    else
        OknoZprav::upozorneni("Při vytváření zálohy databáze došlo k chybě.",
                              "Databáze nebyla zálohována.");
}

void MainWindow::on_actionObnovitDatabaziZeZalohy_triggered() {
    QString verzeDb;
    if (schvalitZtratuNeulozenychZmen()) {
        QString souborZalohy = QFileDialog::getOpenFileName(nullptr, "Otevřít soubor zálohy databáze",
                                                            "./zalohy", "Soubory zálohy (*.bak)");
        if (!souborZalohy.isEmpty()) {
            if (Soubory::obnovitZalohu(souborZalohy, verzeDb)) {
                OknoZprav::informace(QString("Databáze byla obnovena na verzi " + verzeDb
                                             + ". Jestli problém přetrvává, zopakujte obnovu se starší verzí zálohy."));
                // reset databazoveho spojeni
                Databaze::odhlasitOdDatabaze();
                Databaze::prihlasitKDatabazi(Soubory::databaze);

                // jestli v obnovené verzi aktuální uživatel neexistuje, bude odhlášen
                if (!Databaze::existujeUzivatel(prihlasenyUzivatel_->uzivJmeno())) {
                    on_actionOdhlasit_triggered();
                }
                else {
                    zobrazitUdajeAktualnihoPacienta(false);
                    nastavitNeulozeneZmeny(false);
                    nastavitZmenuDatabaze(false);
                    ui->vstupPrijmeni->setFocus();
                    pripravitNaseptavacPrijmeni();
                    zobrazitVerziCiselniku();
                }
            }
            else {
                OknoZprav::upozorneni("Při obnovování databáze došlo k chybě.",
                                      "Databáze nebyla obnovena.");
            }
        }
    }
}

void MainWindow::on_actionUzivatelskaPrirucka_triggered() {
    QDesktopServices::openUrl(QUrl::fromLocalFile(QString(QDir::currentPath() + "/" + Soubory::prirucka)));
}

void MainWindow::on_actionPrehledZmen_triggered() {
    QDesktopServices::openUrl(QUrl::fromLocalFile(QString(QDir::currentPath() + "/" + Soubory::zmeny)));
}

void MainWindow::on_actionOAplikaci_triggered() {
    QMessageBox::about(this, "O aplikaci Závozy", "<div style='text-align: center'>"
                                                  "<p><b>Závozy</b> " + cisloVerze + "</p>"
                                                  "<p>Copyright &copy; 2017 Mgr. Radovan Bednár</p>"
                                                  "<p>Vytvořeno pro použití v Lékárně Nemocnice Znojmo</p>"
                                                  "</div>");
}

void MainWindow::zobrazitPrubehAktualizace() {
    stavovyNapis_->setText("Aktualizuje se číselník:");
    indikator_->show();
    indikator_->setValue(0);
    zpristupnitGui(false);
}

void MainWindow::dokoncitProcesAktualizace() {
    stavovyNapis_->clear();
    indikator_->hide();
    zobrazitVerziCiselniku();
    zpristupnitGui(true);
}

void MainWindow::pripravitNaseptavacPrijmeni() {
    ui->vstupPrijmeni->setCompleter(naseptavac_->naseptavacPrijmeni());
}

void MainWindow::pripravitNaseptavacJmen() {
    ui->vstupJmeno->setCompleter(naseptavac_->naseptavacJmen(ui->vstupPrijmeni->text()));
}

void MainWindow::on_vstupPrijmeni_editingFinished() {
    if (!ui->vstupPrijmeni->text().isEmpty()) {
        pripravitNaseptavacJmen();
        ui->vstupJmeno->setFocus();
        ui->vstupJmeno->selectAll();
    }
}

void MainWindow::on_vstupJmeno_returnPressed() {
    on_tlacitkoVyhledat_clicked();
}

void MainWindow::on_tlacitkoVyhledat_clicked() {
    if (validatorPacient_->jeVyplneno(ui->vstupPrijmeni, false) && validatorPacient_->jeVyplneno(ui->vstupJmeno, false)) {
        if (Databaze::vyhledatPacienta(ui->vstupPrijmeni->text(), ui->vstupJmeno->text(), aktualniPacient_)) {
            zobrazitUdajeAktualnihoPacienta(true);
        }
        else {
            if (OknoZprav::potvrzeni(OknoZprav::Otazka,
                                     QString("Pacient " + Text::kurziva(ui->vstupPrijmeni->text(), ui->vstupJmeno->text())
                                             + " nebyl nalezen v databázi. Přejete si ho založit?"))) {
                if (DialogPacient::otevritNovyPacient(ui->vstupPrijmeni->text(), ui->vstupJmeno->text(), this)) {
                    pripravitNaseptavacPrijmeni();
                    on_tlacitkoVyhledat_clicked();
                }
            }
            else {
                validatorPacient_->zobrazitUpozorneni("Pacient nenalezen v databázi.");
                ui->vstupPrijmeni->setFocus();
                ui->vstupPrijmeni->selectAll();
            }
        }
    }
    else {
        ui->vstupPrijmeni->setFocus();
        ui->vstupPrijmeni->selectAll();
        validatorPacient_->zobrazitUpozorneni(QString("Políčka " + Text::kurziva("Příjmení")
                                                      + " a " + Text::kurziva("Jméno")
                                                      + " musí být vyplněna"));
    }
}

void MainWindow::on_volbaPoznamka_clicked(bool checked) {
    ui->vstupPoznamka->setEnabled(checked);
    ui->vstupPoznamka->setFocus();
}

void MainWindow::on_tlacitkoZrusitVolbuPacienta_clicked() {
    on_actionZrusitVolbuPacienta_triggered();
}

void MainWindow::on_tlacitkoUpravitZvolenehoPacienta_clicked() {
    on_actionUpravitAktualnihoPacienta_triggered();
}

void MainWindow::on_tlacitkoZalozitJmenovcePacienta_clicked() {
    QString prijmeni = ui->vstupPrijmeni->text();
    if (DialogPacient::otevritNovyPacient(prijmeni, ui->vstupJmeno->text(), this)) {
        pripravitNaseptavacPrijmeni();
        on_actionZrusitVolbuPacienta_triggered();
        ui->vstupPrijmeni->setText(prijmeni);
        on_vstupPrijmeni_editingFinished();
    }
}

void MainWindow::zobrazitUdajeAktualnihoPacienta(bool zobrazit) {
    // znepristupni/zpristupni vstupy Prijmeni a Jmeno
    foreach (QLineEdit *policko, QList<QLineEdit*>({ui->vstupPrijmeni, ui->vstupJmeno})) {
        policko->setReadOnly(zobrazit);
        policko->setFrame(!zobrazit);
        if (zobrazit) {
            policko->setStyleSheet("background-color: transparent; font-weight: bold");
            policko->setFocusPolicy(Qt::NoFocus);
        }
        else {
            policko->setStyleSheet("font-weight: normal");
            policko->setFocusPolicy(Qt::StrongFocus);
        }
    }

    // znepristupni/zpristupni tlacitko Vyhledat
    ui->tlacitkoVyhledat->setEnabled(!zobrazit);

    // zpristupni/znepristupni ostatni prvky v sekci Pacient
    foreach (QWidget *prvek, QList<QWidget*>({ui->stalyPopisekAdresa, ui->popisekAdresa,
                                             ui->stalyPopisekTelefony, ui->popisekTelefony,
                                             ui->volbaPoznamka, ui->tlacitkoZrusitVolbuPacienta,
                                             ui->tlacitkoUpravitZvolenehoPacienta, ui->tlacitkoZalozitJmenovcePacienta})) {
        prvek->setEnabled(zobrazit);
    };

    // zpristupni/znepristupni pacientske akce v menu
    foreach (QAction *akce, QList<QAction*>({ui->actionUpravitAktualnihoPacienta,
                                            ui->actionZrusitVolbuPacienta,
                                            ui->actionPrehledZavozuProAktualnihoPacienta})) {
        akce->setEnabled(zobrazit);
    };

    zpristupnitSekciPomucky(zobrazit);

    if (zobrazit) {
        ui->vstupPrijmeni->setText(aktualniPacient_->prijmeni());
        ui->vstupJmeno->setText(aktualniPacient_->jmeno());
        ui->popisekAdresa->setText(aktualniPacient_->adresa());
        ui->popisekTelefony->setText(aktualniPacient_->telefony());

        bool jePoznamka = !aktualniPacient_->poznamka().isEmpty();
        ui->volbaPoznamka->setChecked(jePoznamka);
        ui->vstupPoznamka->setEnabled(jePoznamka);
        ui->vstupPoznamka->setText(aktualniPacient_->poznamka());
        ui->vstupPoznamka->setPlaceholderText("jednorázová poznámka (nebude uložena)");

        mrizkaPomucek_->vyprazdnit();
        naplnitMrizkuPomucek();
        ui->vstupNovaPolozka->setFocus();
    }
    else {
        foreach (QLineEdit *vstup, QList<QLineEdit*>({ui->vstupPrijmeni, ui->vstupJmeno,
                                                 ui->vstupPoznamka})) {
            vstup->clear();
        }

        foreach (QLabel *popisek, QList<QLabel*>({ui->popisekAdresa, ui->popisekTelefony})) {
            popisek->clear();
        }

        ui->volbaPoznamka->setChecked(false);
        ui->vstupPoznamka->setPlaceholderText("");

        mrizkaPomucek_->vyprazdnit();
    }
}

void MainWindow::zpristupnitSekciPomucky(bool zpristupnit) {
    ui->sekcePomucky->setEnabled(zpristupnit);
    pocetKopii_ = 1;
    ui->vstupPocetKopii->setValue(pocetKopii_);
}

void MainWindow::naplnitMrizkuPomucek() {
    QStringList kodyVzp, nazvy, doplnky;

    Databaze::prehledPomucekOdebranychPacientem(aktualniPacient_->id(), kodyVzp, nazvy, doplnky);

    for (int i = 0; i < kodyVzp.size(); i++) {
        if (kodyVzp[i] != "0") {   // vynecha nepomuckove kody
            mrizkaPomucek_->pridatRadek(kodyVzp[i], nazvy[i], doplnky[i]);
            setTabOrder(mrizkaPomucek_->aktualniVstupniPolePoctu(), ui->vstupNovaPolozka);
        }
    }
    setTabOrder(ui->vstupNovaPolozka, ui->tlacitkoVytvoritPruvodku);
}

void MainWindow::on_tlacitkoPridatPolozku_clicked() {
    QString kodVzp = ui->vstupNovaPolozka->text();
    QString nazev, doplnek;
    validatorPomucky_->smazatUpozorneni();

    // u prazdneho nebo nuloveho vstupu vyzve uzivatele k rucnimu zadani polozky
    if (kodVzp.isEmpty() || kodVzp.toInt() == 0) {
        if (rucneZadatNazevPolozky(nazev)) {
            mrizkaPomucek_->pridatRadek("0", nazev.toUpper(), "", true);
            odrolovatNaSpodekSeznamuPomucek();
            aktivovatVstupPocet();
            upravitPoradiVstupnichPoli(mrizkaPomucek_->aktualniVstupniPolePoctu());
            smazatVstupNovaPolozka();
        }
        else {
            smazatVstupNovaPolozka();
            aktivovatVstupNovaPolozka();
        }
    }
    // jinak vyhleda zadany kod v databazi
    else {
        if (Databaze::vyhledatPomucku(kodVzp, nazev, doplnek)) {
            if (mrizkaPomucek_->neniDuplicita(kodVzp)) {
                mrizkaPomucek_->pridatRadek(kodVzp, nazev, doplnek, true);
                odrolovatNaSpodekSeznamuPomucek();
                aktivovatVstupPocet();
                upravitPoradiVstupnichPoli(mrizkaPomucek_->aktualniVstupniPolePoctu());
                smazatVstupNovaPolozka();
            }
            else {  // polozka s danym kodem uz je v seznamu
                WidgetVstupniPolePoctu *vstupPocet = mrizkaPomucek_->vstupniPolePoctuPolozky(kodVzp);
                // pokud metoda vrati platny pointer na vstupni pole a dane pole je nevyplnene, aktivuje se toto pole
                if (vstupPocet && !vstupPocet->value()) {
                    ui->rolovatelnaOblastSeznamPomucek->ensureWidgetVisible(vstupPocet);
                    vstupPocet->setFocus();
                    upravitPoradiVstupnichPoli(vstupPocet);
                    smazatVstupNovaPolozka();
                }
                // jestli je dane vstupni pole vyplneno, pouze se zobrazi upozorneni
                else {
                    validatorPomucky_->zobrazitUpozorneni(QString("Položka s kódem " + Text::kurziva(kodVzp)
                                                                  + " se už nachází v seznamu pomůcek"));
                    aktivovatVstupNovaPolozka();
                }
            }
        }
        else {  // kod nenalezen v databazi
            validatorPomucky_->zobrazitUpozorneni(QString("Číselník neobsahuje položku s kódem " + Text::kurziva(kodVzp)));
            aktivovatVstupNovaPolozka();
        }
    }
}

bool MainWindow::rucneZadatNazevPolozky(QString &nazev) {
    bool vyplneno;
    nazev = QInputDialog::getText(this, "Jiná položka než pomůcka",
                                  "Zadejte název položky k závozu:", QLineEdit::Normal,
                                  "", &vyplneno);

    return (vyplneno && !nazev.isEmpty());
}

void MainWindow::odrolovatNaSpodekSeznamuPomucek() {
    ui->rolovatelnaOblastSeznamPomucek->widget()->adjustSize();
    qApp->processEvents();
    QScrollBar *lista = ui->rolovatelnaOblastSeznamPomucek->verticalScrollBar();
    lista->setValue(lista->maximum());
}

void MainWindow::aktivovatVstupPocet() {
    mrizkaPomucek_->aktualniVstupniPolePoctu()->setFocus();
}

void MainWindow::upravitPoradiVstupnichPoli(QWidget *vstupPocet) {
    setTabOrder(vstupPocet, ui->vstupNovaPolozka);
    setTabOrder(ui->vstupNovaPolozka, ui->tlacitkoVytvoritPruvodku);
}

void MainWindow::odstranitPosledniPolozku() {
    mrizkaPomucek_->odstranitPosledniRadek();
    setTabOrder(mrizkaPomucek_->aktualniVstupniPolePoctu(), ui->vstupNovaPolozka);
}

void MainWindow::smazatVstupNovaPolozka() {
    ui->vstupNovaPolozka->clear();
}

void MainWindow::aktivovatVstupNovaPolozka() {
    ui->vstupNovaPolozka->selectAll();
    ui->vstupNovaPolozka->setFocus();
}

void MainWindow::on_vstupPocetKopii_valueChanged(int hodnota) {
    if (hodnota == 1) {
        ui->tlacitkoVytvoritPruvodku->setText("Vytvořit průvodku a uložit");
    }
    else {
        ui->tlacitkoVytvoritPruvodku->setText("Vytvořit průvodky a uložit");
    }
    pocetKopii_ = hodnota;
}

void MainWindow::on_tlacitkoVytvoritPruvodku_clicked() {
    QList<PolozkaZavozu> zvolenePomucky = mrizkaPomucek_->zvolenePomucky();
    int pacId = aktualniPacient_->id();
    QDate datumZavozu = ui->vstupDatumZavozu->date();
    validatorPomucky_->smazatUpozorneni();

    if (!zvolenePomucky.isEmpty()) {
        // vlozit zaznam o zavozu do tabulky Zavoz
        if (Databaze::ulozitZavoz(prihlasenyUzivatel_->uzivJmeno(), aktualniPacient_->adresa(), pacId, datumZavozu)) {
            // vlozit jednotlive polozky zavozu do tabulky polozkaZavozu
            foreach (auto polozka, zvolenePomucky) {
                if (!Databaze::ulozitPolozkuZavozu(polozka.pocet(), polozka.kodVzp(), pacId, datumZavozu)) {
                    OknoZprav::chybaPriZapisuDoDb(QString("Položka závozu s kódem " + QString::number(polozka.kodVzp())
                                                          + " nebyla zapsaná."));
                }
            }

            // ulozit nevytistenou pruvodku zavozu
            QString poznamka = (ui->volbaPoznamka->isChecked() ? ui->vstupPoznamka->text()
                                                               : "");
            while (pocetKopii_ > 0) {
                nevytistene_.append(Zavoz(aktualniPacient_, poznamka, zvolenePomucky));
                pocetKopii_--;
            }
            ulozitSeznamPruvodekDoSouboru(nevytistene_, Soubory::nevytistene);
            pocetKopii_ = 1;

            // aktualizovat pocitadlo nevytistenych pruvodek
            zobrazitPocetNevytistenychPruvodek();

            // resetovat priznak neulozenych zmen, nastavit zmenu db. a zvolit noveho pacienta
            nastavitNeulozeneZmeny(false);
            nastavitZmenuDatabaze(true);
            on_actionZrusitVolbuPacienta_triggered();
        }
        else {
            validatorPomucky_->zobrazitUpozorneni("Pro daného pacienta už byl pořízen závoz se stejným datem. Zvolte jiné datum závozu");
            OknoZprav::chybaPriZapisuDoDb("Závoz nebyl uložen.");
        }
    }
    else {
        validatorPomucky_->zobrazitUpozorneni("Seznam zboží neobsahuje žádnou položku s nenulovým počtem");
    }
}

void MainWindow::zobrazitPocetNevytistenychPruvodek() {
    int pocet = nevytistene_.size();

    ui->popisekPocetPruvodek->setText(QString::number(pocet));
    ui->popisekPocetPruvodek->setStyleSheet((bool)pocet ? "color: red" : "");
    ui->sekcePruvodky->setEnabled(pocet);
    ui->actionVytisknoutNevytistenePruvodky->setEnabled((bool)pocet);
    ui->actionZobrazitPrehledNevytistenychPruvodek->setEnabled((bool)pocet);
}

void MainWindow::on_tlacitkoVytisknoutPruvodky_clicked() {
    TiskarnaPruvodek *tiskarna = new TiskarnaPruvodek(nevytistene_);
    ulozitSeznamVytistenychPruvodek();
    vyprazdnitSeznamZavozu();   // musi nasledovat po ulozeni souboru
    nacistDnesVytistenePruvodky();
    delete tiskarna;
    QDesktopServices::openUrl(QUrl::fromLocalFile(QString(QDir::currentPath() + "/" + Soubory::tisk)));
}

void MainWindow::vyprazdnitSeznamZavozu() {
    nevytistene_.clear();
    zobrazitPocetNevytistenychPruvodek();
}

void MainWindow::nastavitNeulozeneZmeny(bool zmeneno) {
    validatorPomucky_->smazatUpozorneni();
    neulozeneZmeny_ = zmeneno;
}

void MainWindow::nastavitZmenuDatabaze(bool zmeneno) {
    databazeZmenena_ = zmeneno;
    ui->actionSpustitOkamzitouZalohuDatabaze->setEnabled(zmeneno);
}

void MainWindow::vycentrovatOkno() {
    int vyskaRamuOkna = this->frameGeometry().height() - this->height();

    this->resize(this->width(),
                 QDesktopWidget().availableGeometry().height() - vyskaRamuOkna);
    this->move((QDesktopWidget().availableGeometry().width() - this->frameGeometry().width()) / 2,
               0);
}

void MainWindow::pripravitVstupDatumZavozu() {
    ui->vstupDatumZavozu->setMinimumDate(QDate::currentDate());
    ui->vstupDatumZavozu->setDate(Datum::denNejblizsihoZavozu());
    ui->vstupDatumZavozu->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    QTextCharFormat dnyZavozu;
    dnyZavozu.setFontWeight(QFont::Bold);
    ui->vstupDatumZavozu->calendarWidget()->setWeekdayTextFormat(Qt::Wednesday, dnyZavozu);
    ui->vstupDatumZavozu->calendarWidget()->setWeekdayTextFormat(Qt::Friday, dnyZavozu);
}

void MainWindow::nacistNevytistenePruvodky() {
    nacistSeznamPruvodekZeSouboru(Soubory::nevytistene, nevytistene_);
    zobrazitPocetNevytistenychPruvodek();
}

void MainWindow::nacistDnesVytistenePruvodky() {
    if (nacistSeznamPruvodekZeSouboru(Soubory::souborDnesVytistene(), dnesVytistene_))
        ui->actionOpetovneZaraditPruvodkyDoTiskoveFronty->setEnabled(true);
}

void MainWindow::vyzvatKZalozeniPrvnihoUzivatele() {
    OknoZprav::upozorneni("Databáze neobsahuje žádné aktivní uživatele.",
                          "Před prvním použitím aplikace je nutno založit alespoň "
                          "jednoho uživatele s\u00A0administrátorskými právy.");

    ui->actionPrihlasit->setEnabled(false);
    ui->actionPridatUzivatele->setEnabled(true);

    if (DialogNovyUzivatel::otevrit(this)) {
        vyzvatKPrihlaseniPrvnihoUzivatele();
    }
}

void MainWindow::vyzvatKPrihlaseniPrvnihoUzivatele() {
    ui->actionPrihlasit->setEnabled(true);
    ui->actionPridatUzivatele->setEnabled(false);

    QTimer::singleShot(100, this, SLOT(on_actionPrihlasit_triggered()));
}

void MainWindow::vyzvatKNahraniCiselniku() {
    OknoZprav::upozorneni("Číselník zdravotních pomůcek v\u00A0databázi je prázdný.",
                          "Před prvním použitím aplikace je nutno provést aktualizaci číselníku.");
}

void MainWindow::ukoncitAplikaciSChybou() {
    QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
}

void MainWindow::zobrazitVerziCiselniku() {
    QString verzeCiselniku, uzivatel;
    QDate datumAktualizace;

    if (Databaze::verzeCiselniku(verzeCiselniku, datumAktualizace, uzivatel)) {
        QString textDatum = datumAktualizace.toString("d. M. yyyy");
        QString textPopisku = "používaná verze číselníku: " + verzeCiselniku
                + ", aktualizováno " + textDatum + " uživatelem " + uzivatel;

        if (datumAktualizace < QDate::currentDate().addMonths(-2)) {
            textPopisku.replace(textDatum, Text::cervene(textDatum));
            ui->popisekVerzeCiselniku->setToolTip("Číselník ZP nebyl aktualizován víc než dva měsíce. Proveďte aktualizaci");
        }

        ui->popisekVerzeCiselniku->setText(textPopisku);
    }
    else {
        QTimer::singleShot(100, this, &MainWindow::vyzvatKNahraniCiselniku);
    }
}

void MainWindow::povolitUzivatelskeAkce(bool povoleno) {
    ui->centralWidget->setEnabled(povoleno);
    ui->actionPrihlasit->setEnabled(!povoleno);
    ui->actionZmenitHeslo->setEnabled(povoleno);
    ui->actionOdhlasit->setEnabled(povoleno);
    ui->menuPacient->setEnabled(povoleno);
    ui->menuTisk->setEnabled(povoleno);
    ui->menuPrehledy->setEnabled(povoleno);
    ui->menuUdrzba->setEnabled(povoleno);
    ui->actionSpustitOkamzitouZalohuDatabaze->setEnabled(databazeZmenena_);
}

void MainWindow::povolitAdministratorskeAkce(bool povoleno) {
    ui->actionPridatUzivatele->setEnabled(povoleno);
    ui->actionUpravitSmazatUzivatele->setEnabled(povoleno);
    ui->actionObnovitDatabaziZeZalohy->setEnabled(povoleno);
}

void MainWindow::zpristupnitGui(bool zpristupnit) {
    ui->centralWidget->setEnabled(zpristupnit);
    ui->menuBar->setEnabled(zpristupnit);
}

bool MainWindow::ulozitSeznamPruvodekDoSouboru(QList<Zavoz> &seznamPruvodek, QString souborPruvodek) {
    QFile saveFile(souborPruvodek);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        OknoZprav::upozorneni(QString("Chyba při otevírání souboru " + Text::kurziva(souborPruvodek) + "."),
                              "Seznam průvodek nemohl být uložen.");
        return false;
    }

    QJsonArray poleZavozu;
    foreach (Zavoz zavoz, seznamPruvodek) {
        QJsonObject objektZavoz;
        zavoz.zapsatJson(objektZavoz);
        poleZavozu.append(objektZavoz);
    }

    QJsonDocument dokument(poleZavozu);

    return (saveFile.write(dokument.toJson()) != -1);
}

bool MainWindow::nacistSeznamPruvodekZeSouboru(QString souborPruvodek, QList<Zavoz> &seznamPruvodek) {
    seznamPruvodek.clear();
    QFile loadFile(souborPruvodek);

    if (!loadFile.open(QIODevice::ReadOnly)) {
        OknoZprav::upozorneni(QString("Při otevírání souboru " + Text::kurziva(souborPruvodek) + " došlo k chybě."),
                              "Seznam průvodek nemohl být načten.");
        return false;
    }

    QByteArray saveData = loadFile.readAll();

    QJsonParseError chybaParsovani;
    QJsonDocument dokument(QJsonDocument::fromJson(saveData, &chybaParsovani));

    if (chybaParsovani.error) {
        OknoZprav::upozorneni(QString("Při čtení souboru " + Text::kurziva(souborPruvodek)
                                      + " došlo k následující chybě: " + Text::kurziva(chybaParsovani.errorString())),
                              "Soubor je poškozen. Seznam průvodek nemohl být načten.");
        return false;
    }

    QJsonArray poleZavozu = dokument.array();
    for (int i = 0; i < poleZavozu.size(); i++) {
        QJsonObject objektZavoz = poleZavozu[i].toObject();
        Zavoz zavoz;
        zavoz.cistJson(objektZavoz);
        seznamPruvodek.append(zavoz);
    }

    return true;
}

bool MainWindow::ulozitSeznamVytistenychPruvodek() {
    bool uspech;
    // jestli se v dany den jeste netisklo, vytvori soubor s novymi pruvodkami
    if (!Soubory::existujiDnesVytistene()) {
        uspech = ulozitSeznamPruvodekDoSouboru(nevytistene_, Soubory::souborDnesVytistene());
    }
    // jinak nacte soubor s uz vytistenymi pruvodkami, prida k nim nove a ulozi do jednoho souboru
    else {
        nacistSeznamPruvodekZeSouboru(Soubory::souborDnesVytistene(), dnesVytistene_);
        dnesVytistene_.append(nevytistene_);

        uspech = ulozitSeznamPruvodekDoSouboru(dnesVytistene_, Soubory::souborDnesVytistene());
    }

    if (uspech) {
        Soubory::odstranitNevytistene();
    }

    return uspech;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (schvalitZtratuNeulozenychZmen()) {
        if (prihlasenyUzivatel_)
            delete prihlasenyUzivatel_;
        Soubory::odstranitTiskovySoubor();
        if (databazeZmenena_)
            Soubory::zalohovatDatabazi();
        event->accept();
    }
    else {
        event->ignore();
    }
}

bool MainWindow::schvalitZtratuNeulozenychZmen() {
    if (neulozeneZmeny_)
        return OknoZprav::potvrzeni(OknoZprav::Varovani,
                                    "Změny v seznamu pomůcek provedené u aktuálního pacienta "
                                    "nebudou uloženy. Chcete pokračovat?");
    else
        return true;
}
