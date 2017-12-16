#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCalendarWidget>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QMainWindow>
#include <QProgressBar>
#include <QScrollBar>
#include <QSpinBox>
#include <QTimer>
#include "AktualizatorCiselniku.h"
#include "Databaze.h"
#include "DialogPrehled.h"
#include "DialogNovyUzivatel.h"
#include "DialogPacient.h"
#include "DialogPrihlaseni.h"
#include "DialogSeznamPruvodek.h"
#include "DialogUpravitSmazatUzivatele.h"
#include "FiltrUdalostiVstupNovaPolozka.h"
#include "globalni.h"
#include "Naseptavac.h"
#include "Pacient.h"
#include "TiskarnaPruvodek.h"
#include "Uzivatel.h"
#include "ValidatorVstupu.h"
#include "WidgetSubsekcePomuckySeznam.h"
#include "Zavoz.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    // menu Uživatel
    void on_actionPrihlasit_triggered();
    void on_actionZmenitHeslo_triggered();
    void on_actionPridatUzivatele_triggered();
    void on_actionUpravitSmazatUzivatele_triggered();
    void on_actionOdhlasit_triggered();
    void on_actionUkoncitAplikaci_triggered();

    // menu Pacient
    void on_actionNovyPacient_triggered();
    void on_actionUpravitPacienta_triggered();
    void on_actionUpravitAktualnihoPacienta_triggered();
    void on_actionZrusitVolbuPacienta_triggered();
    void on_actionZobrazitSeznamVsechPacientu_triggered();

    // menu Tisk
    void on_actionVytisknoutNevytistenePruvodky_triggered();
    void on_actionZobrazitPrehledNevytistenychPruvodek_triggered();
    void on_actionOpetovneZaraditPruvodkyDoTiskoveFronty_triggered();

    // menu Přehledy
    void on_actionPrehledZavozuProPacienta_triggered();
    void on_actionPrehledZavozuProAktualnihoPacienta_triggered();
    void on_actionPrehledZavozuVychystanychUzivatelem_triggered();
    void on_actionPrehledZavozuJednotlivePomucky_triggered();
    void on_actionPrehledVsechZavozuZaObdobi_triggered();

    // menu Údržba
    void on_actionOtevritWebstrankuSCiselnikyPomucek_triggered();
    void on_actionAktualizovatCiselnikPomucekZeSouboru_triggered();
    void on_actionSpustitOkamzitouZalohuDatabaze_triggered();
    void on_actionObnovitDatabaziZeZalohy_triggered();

    // menu Nápověda
    void on_actionUzivatelskaPrirucka_triggered();
    void on_actionPrehledZmen_triggered();
    void on_actionOAplikaci_triggered();

    // proces aktualizace ciselniku
    void zobrazitPrubehAktualizace();
    void dokoncitProcesAktualizace();

    // hlavni formular - sekce Pacient
    void pripravitNaseptavacPrijmeni();
    void pripravitNaseptavacJmen();
    void on_vstupPrijmeni_editingFinished();
    void on_vstupJmeno_returnPressed();
    void on_tlacitkoVyhledat_clicked();
    void on_volbaPoznamka_clicked(bool checked);
    void on_tlacitkoZrusitVolbuPacienta_clicked();
    void on_tlacitkoUpravitZvolenehoPacienta_clicked();
    void on_tlacitkoZalozitJmenovcePacienta_clicked();
    void zobrazitUdajeAktualnihoPacienta(bool zobrazit);

    // hlavni formular - sekce Pomucky
    void zpristupnitSekciPomucky(bool zpristupnit);
    void naplnitMrizkuPomucek();
    void on_tlacitkoPridatPolozku_clicked();
    bool rucneZadatNazevPolozky(QString &nazev);
    void odrolovatNaSpodekSeznamuPomucek();
    void aktivovatVstupPocet();
    void upravitPoradiVstupnichPoli(QWidget *vstupPocet);
    void odstranitPosledniPolozku();
    void smazatVstupNovaPolozka();
    void aktivovatVstupNovaPolozka();
    void on_vstupPocetKopii_valueChanged(int hodnota);
    void on_tlacitkoVytvoritPruvodku_clicked();

    // hlavni formular - sekce Pruvodky
    void zobrazitPocetNevytistenychPruvodek();
    void on_tlacitkoVytisknoutPruvodky_clicked();
    void vyprazdnitSeznamZavozu();

    // provedene zmeny
    void nastavitNeulozeneZmeny(bool zmeneno);
    void nastavitZmenuDatabaze(bool zmeneno);

private:
    // spousteni aplikace
    void vycentrovatOkno();
    void pripravitVstupDatumZavozu();
    void nacistNevytistenePruvodky();
    void nacistDnesVytistenePruvodky();
    void vyzvatKZalozeniPrvnihoUzivatele();
    void vyzvatKPrihlaseniPrvnihoUzivatele();
    void vyzvatKNahraniCiselniku();
    void ukoncitAplikaciSChybou();

    // prihlasovani uzivatele
    void zobrazitVerziCiselniku();

    // zmeny GUI
    void povolitUzivatelskeAkce(bool povoleno);
    void povolitAdministratorskeAkce(bool povoleno);
    void zpristupnitGui(bool zpristupnit);

    // ukladani/nacitani pruvodek
    bool ulozitSeznamPruvodekDoSouboru(QList<Zavoz> &seznamPruvodek, QString souborPruvodek);
    bool nacistSeznamPruvodekZeSouboru(QString souborPruvodek, QList<Zavoz> &seznamPruvodek);
    bool ulozitSeznamVytistenychPruvodek();

    // ukoncovani aplikace
    void closeEvent(QCloseEvent *event);
    bool schvalitZtratuNeulozenychZmen();

    Naseptavac *naseptavac_;
    Ui::MainWindow *ui;

    Uzivatel *prihlasenyUzivatel_ = nullptr;
    Pacient *aktualniPacient_ = nullptr;

    WidgetSubsekcePomuckySeznam *mrizkaPomucek_ = nullptr;

    ValidatorVstupu *validatorPacient_;
    ValidatorVstupu *validatorPomucky_;
    bool neulozeneZmeny_ = false;
    bool databazeZmenena_ = false;
    int pocetKopii_ = 1;
    QList<Zavoz> nevytistene_;
    QList<Zavoz> dnesVytistene_;

    QLabel *stavovyNapis_;
    QProgressBar *indikator_;

    QList<QLineEdit*> vstupyPrijmeniAJmeno_;
};

#endif // MAINWINDOW_H
