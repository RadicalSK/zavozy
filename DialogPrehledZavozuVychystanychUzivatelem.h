#ifndef DIALOGPREHLEDZAVOZUVYCHYSTANYCHUZIVATELEM_H
#define DIALOGPREHLEDZAVOZUVYCHYSTANYCHUZIVATELEM_H

#include "AbstraktniDialogPrehled.h"

class DialogPrehledZavozuVychystanychUzivatelem : public AbstraktniDialogPrehled
{
    Q_OBJECT

public:
    static void otevrit(QWidget *rodic);

private slots:
    void zvolitUzivatele(int cisloZaznamu);

private:
    DialogPrehledZavozuVychystanychUzivatelem(QWidget *rodic = 0);

    virtual bool vyhledatZaznamy() override;
    virtual QString navrhnoutNazevCsvSouboru() override;

    QComboBox *comboBoxUzivatel_;
    QLabel *popisekUzivatel_;
};

#endif // DIALOGPREHLEDZAVOZUVYCHYSTANYCHUZIVATELEM_H
