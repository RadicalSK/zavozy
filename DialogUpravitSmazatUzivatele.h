#ifndef DIALOGUPRAVITSMAZATUZIVATELE_H
#define DIALOGUPRAVITSMAZATUZIVATELE_H

#include <QDialog>
#include <QKeyEvent>
#include <QLineEdit>
#include "Databaze.h"
#include "globalni.h"
#include "Uzivatel.h"
#include "ValidatorVstupu.h"

namespace Ui {
class DialogUpravitSmazatUzivatele;
}

class DialogUpravitSmazatUzivatele : public QDialog
{
    Q_OBJECT

public:
    static bool otevritZmenaHesla(Uzivatel *uzivatel);
    static bool otevritUpravaUzivatele(Uzivatel *uzivatel);

private slots:
    void on_seznamUzivJmeno_currentIndexChanged(const QString &uzivJmeno);
    void on_vstupCeleJmeno_editingFinished();
    void on_vstupStareHeslo_editingFinished();
    void on_vstupNoveHeslo_editingFinished();
    void on_vstupNoveHeslo2_editingFinished();
    void on_volbaSmazatUzivatele_toggled(bool checked);

    void on_tlacitkoZrusit_clicked();
    void on_tlacitkoUlozit_clicked();

private:
    explicit DialogUpravitSmazatUzivatele(Uzivatel *uzivatel, bool zmenaHesla = true, QWidget *rodic = 0);
    ~DialogUpravitSmazatUzivatele();

    bool validovatVstupCeleJmeno();
    bool validovatVstupStareHeslo();
    bool validovatVstupNoveHeslo(QLineEdit *policko);

    void keyPressEvent(QKeyEvent *udalost) override;

    Uzivatel *uzivatel_;
    bool zmenaHesla_;

    Ui::DialogUpravitSmazatUzivatele *ui;
    ValidatorVstupu *validator_;

    QList<QLineEdit*> vstupyHesel_;
};

#endif // DIALOGUPRAVITSMAZATUZIVATELE_H
