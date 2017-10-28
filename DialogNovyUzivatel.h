#ifndef DIALOGNOVYUZIVATEL_H
#define DIALOGNOVYUZIVATEL_H

#include <QDialog>
#include <QKeyEvent>
#include <QLineEdit>
#include "Databaze.h"
#include "Uzivatel.h"
#include "ValidatorVstupu.h"

namespace Ui {
class DialogNovyUzivatel;
}

class DialogNovyUzivatel : public QDialog
{
    Q_OBJECT

public:
    static bool otevrit();

private slots:
    void on_vstupUzivJmeno_textChanged(const QString &text);
    void on_vstupUzivJmeno_editingFinished();
    void on_vstupCeleJmeno_editingFinished();
    void on_vstupHeslo_editingFinished();
    void on_vstupHeslo2_editingFinished();

    void on_tlacitkoZrusit_clicked();
    void on_tlacitkoZalozit_clicked();

private:
    explicit DialogNovyUzivatel(QWidget *rodic = 0);
    ~DialogNovyUzivatel();

    bool validovatVstupUzivJmeno();
    bool validovatVstupCeleJmeno();
    bool validovatVstupHeslo();
    bool validovatVstupHeslo2();
    bool jeZvolenTypUzivatele();

    void keyPressEvent(QKeyEvent *udalost) override;

    Ui::DialogNovyUzivatel *ui;
    ValidatorVstupu *validator_;

    QList<QLineEdit*> vsechnaPolicka_;
};

#endif // DIALOGNOVYUZIVATEL_H
