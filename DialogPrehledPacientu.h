#ifndef DIALOGPREHLEDPACIENTU_H
#define DIALOGPREHLEDPACIENTU_H

#include "AbstraktniDialogPrehled.h"
#include "DialogPacient.h"

class DialogPrehledPacientu : public AbstraktniDialogPrehled
{
public:
    static bool otevrit(QWidget *rodic);

private slots:
    void upravitZvolenehoPacienta(const QModelIndex &index);

private:
    DialogPrehledPacientu(QWidget *rodic);

    virtual bool vyhledatZaznamy() override;
    virtual QString navrhnoutNazevCsvSouboru() override;

    static bool provedenyZmenyVDb_;
};

#endif // DIALOGPREHLEDPACIENTU_H
