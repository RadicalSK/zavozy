#ifndef ABSTRAKTNIDIALOGPREHLED_H
#define ABSTRAKTNIDIALOGPREHLED_H

#include <QCalendarWidget>
#include <QDate>
#include <QDialog>
#include <QFileDialog>
#include <QKeyEvent>
#include <QSortFilterProxyModel>
#include <QSqlQueryModel>
#include "Databaze.h"
#include "globalni.h"
#include "Naseptavac.h"
#include "ValidatorVstupu.h"

namespace Ui {
    class AbstraktniDialogPrehled;
}

class AbstraktniDialogPrehled : public QDialog
{
    Q_OBJECT

public:    
    explicit AbstraktniDialogPrehled(QWidget *rodic = 0);
    ~AbstraktniDialogPrehled();

protected slots:
    void on_vstupDatumOd_dateChanged(const QDate &datum);
    void on_vstupDatumDo_dateChanged(const QDate &datum);
    void on_comboBoxObdobi_currentIndexChanged(int index);
    void on_tlacitkoVyhledat_clicked();

    void on_tlacitkoExportovatCsv_clicked();
    void on_tlacitkoZavrit_clicked();

protected:
    void keyPressEvent(QKeyEvent *udalost) override;

    enum CasoveObdobi {
        LibovolneObodobi, DenNejblizsihoZavozu, PredchoziMesic, PredchoziKvartal, PredchoziRok, CelaDoba
    };

    void resetovatComboBoxObdobi();
    bool casoveRozpetiValidni();
    void povolitVyhledavaciTlacitko(bool povolit);

    virtual bool vyhledatZaznamy() = 0;
    void zobrazitZaznamy();
    void zpristupnitTabulku(bool zpristupnit = true);
    void vyprazdnitTabulku();

    virtual QString navrhnoutNazevCsvSouboru() = 0;
    QString casoveRozpetiProNazev();
    QString ziskatNazevCsvSouboru();
    QString vytvoritTextovaDataZTabulky();

    Naseptavac *naseptavac_;
    Ui::AbstraktniDialogPrehled *ui;
    ValidatorVstupu *validator_;

    const QDate zacatek_ = QDate(2017, 9, 1);
    const QDate dnes_ = QDate::currentDate();
    QDate datumOd_ = dnes_.addDays(-7);
    QDate datumDo_ = dnes_.addDays(7);

    QSqlQueryModel *queryModel_ = new QSqlQueryModel(this);
    QSortFilterProxyModel *filterModel_ = new QSortFilterProxyModel(this);
};

#endif // ABSTRAKTNIDIALOGPREHLED_H
