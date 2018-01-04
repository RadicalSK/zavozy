#ifndef DIALOGPREHLEDZAVOZUPROPACIENTA_H
#define DIALOGPREHLEDZAVOZUPROPACIENTA_H

#include "AbstraktniDialogPrehled.h"
#include "Pacient.h"

class DialogPrehledZavozuProPacienta : public AbstraktniDialogPrehled
{
    Q_OBJECT

public:
    static void otevrit(QWidget *rodic);
    static void otevritZavozyAktualnihoPacienta(Pacient *pacient, QWidget *rodic);

private slots:
    void prepnoutNaVstupJmeno();
    void vyhledatPacientaVDatabazi();
    void zrusitVolbuPacienta();

private:
    DialogPrehledZavozuProPacienta(QWidget *rodic = 0);
    DialogPrehledZavozuProPacienta(Pacient *pacient, QWidget *rodic = 0);

    void zpristupnitVstupy(bool zpristupnit);
    virtual bool vyhledatZaznamy() override;
    virtual QString navrhnoutNazevCsvSouboru() override;

    int pacId_ = 0;

    QLabel *stalyPopisekPrijmeni_, *stalyPopisekJmeno_;
    QLineEdit *vstupPrijmeni_, *vstupJmeno_;
    QPushButton *tlacitkoNajitVDatabazi_, *tlacitkoJinyPacient_;
};

#endif // DIALOGPREHLEDZAVOZUPROPACIENTA_H
