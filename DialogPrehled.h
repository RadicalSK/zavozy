#ifndef DIALOGPREHLED_H
#define DIALOGPREHLED_H

#include <QCalendarWidget>
#include <QDate>
#include <QDialog>
#include <QFileDialog>
#include <QKeyEvent>
#include <QSortFilterProxyModel>
#include <QSqlQueryModel>
#include <QTextStream>
#include "DialogPacient.h"
#include "globalni.h"
#include "Naseptavac.h"
#include "Pacient.h"
#include "ValidatorVstupu.h"

namespace Ui {
class DialogPrehled;
}

class DialogPrehled : public QDialog
{
    Q_OBJECT

public:
    enum TypPrehledu {
        ZavozyPacienta, ZavozyUzivatele, PomuckaZaObdobi, ZavozyZaObdobi, Pacienti
    };

    static bool otevrit(TypPrehledu typ, QWidget *rodic);
    static bool otevritZavozyAktualnihoPacienta(Pacient *pacient, QWidget *rodic);

private slots:
    void on_vstupPrijmeni_textChanged(const QString &text);
    void on_vstupPrijmeni_returnPressed();
    void on_vstupJmeno_textChanged(const QString &text);
    void on_vstupJmeno_returnPressed();
    void on_tlacitkoNajitPacienta_clicked();
    void on_tlacitkoJinyPacient_clicked();

    void on_vstupKodVzp_textChanged(const QString &text);
    void on_vstupKodVzp_returnPressed();
    void on_tlacitkoNajitPomucku_clicked();

    void on_comboBoxUzivatel_currentIndexChanged(int index);

    void on_vstupDatumOd_dateChanged(const QDate &datum);
    void on_vstupDatumDo_dateChanged(const QDate &datum);
    void on_comboBoxObdobi_currentIndexChanged(int index);
    void on_tlacitkoVyhledat_clicked();

    void on_tabulka_doubleClicked(const QModelIndex &index);
    void on_tlacitkoExportovatCsv_clicked();
    void on_tlacitkoZavrit_clicked();

private:
    explicit DialogPrehled(TypPrehledu typ, QWidget *rodic = 0);
    explicit DialogPrehled(Pacient *pacient, QWidget *rodic = 0);
    ~DialogPrehled();

    void keyPressEvent(QKeyEvent *udalost) override;

    enum CasoveObdobi {
        LibovolneObodobi, PredchoziMesic, PredchoziKvartal, PredchoziRok, CelaDoba
    };

    void zpristupnitVstupy(bool zpristupnit);
    void resetovatComboBoxObdobi();
    bool casoveRozpetiValidni();
    void povolitVyhledavaciTlacitko(bool povolit);

    void zobrazitZaznamy();
    bool vyhledatZaznamy();
    void zpristupnitTabulku(bool zpristupnit = true);
    void vyprazdnitTabulku();

    QDate prvniDenPredchozihoMesice();
    QDate posledniDenPredchozihoMesice();
    QDate prvniDenPredchozihoKvartalu();
    QDate posledniDenPredchozihoKvartalu();
    QDate prvniDenPrechodzihoRoku();
    QDate posledniDenPredchozihoRoku();

    QString ziskatNazevCsvSouboru();
    QString vytvoritTextovaDataZTabulky();

    TypPrehledu typ_;
    Naseptavac *naseptavac_;
    Ui::DialogPrehled *ui;

    ValidatorVstupu *validator_;

    int pacId_ = 0;

    static bool provedenyZmenyVDb_;

    const QDate zacatek_ = QDate(2017, 9, 1);
    const QDate dnes_ = QDate::currentDate();
    QDate datumOd_ = dnes_.addDays(-7);
    QDate datumDo_ = dnes_.addDays(7);

    QSqlQueryModel *queryModel_ = new QSqlQueryModel(this);
    QSortFilterProxyModel *filterModel_ = new QSortFilterProxyModel(this);
};

#endif // DIALOGPREHLED_H
