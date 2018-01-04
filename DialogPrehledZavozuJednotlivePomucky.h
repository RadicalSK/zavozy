#ifndef DIALOGPREHLEDZAVOZUJEDNOTLIVEPOMUCKY_H
#define DIALOGPREHLEDZAVOZUJEDNOTLIVEPOMUCKY_H

#include "AbstraktniDialogPrehled.h"

class DialogPrehledZavozuJednotlivePomucky : public AbstraktniDialogPrehled
{
    Q_OBJECT

public:
    static void otevrit(QWidget *rodic);

private slots:
    void zpristupnitVyhledavaciTlacitko(QString zadanyText);
    void vyhledatPomuckuVDatabazi();

private:
    DialogPrehledZavozuJednotlivePomucky(QWidget *rodic = 0);

    virtual bool vyhledatZaznamy() override;
    virtual QString navrhnoutNazevCsvSouboru() override;

    QLabel *stalyPopisekKodVzp_;
    QLineEdit *vstupKodVzp_;
    QPushButton *tlacitkoNajitVCiselniku_;
    QLabel *popisekPomucka_;
};

#endif // DIALOGPREHLEDZAVOZUJEDNOTLIVEPOMUCKY_H
