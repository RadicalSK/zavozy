#ifndef DIALOGPREHLEDVSECHZAVOZUZAOBDOBI_H
#define DIALOGPREHLEDVSECHZAVOZUZAOBDOBI_H

#include <QRadioButton>
#include "AbstraktniDialogPrehled.h"

class DialogPrehledVsechZavozuZaObdobi : public AbstraktniDialogPrehled
{
    Q_OBJECT

public:
    static void otevrit(QWidget *rodic);

private slots:
    void volbaZmenena(bool zaskrtnuto);

private:
    DialogPrehledVsechZavozuZaObdobi(QWidget *rodic);

    virtual bool vyhledatZaznamy() override;
    virtual QString navrhnoutNazevCsvSouboru() override;

    QLabel *stalyPopisekZaznamyZobrazit_;
    QRadioButton *volbaPoPacientech_, *volbaPoPomuckach_;
};

#endif // DIALOGPREHLEDVSECHZAVOZUZAOBDOBI_H
