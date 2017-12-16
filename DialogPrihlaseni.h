#ifndef DIALOGPRIHLASENI_H
#define DIALOGPRIHLASENI_H

#include <QDialog>
#include <QKeyEvent>
#include "Databaze.h"
#include "Uzivatel.h"
#include "ValidatorVstupu.h"

namespace Ui {
class DialogPrihlaseni;
}

class DialogPrihlaseni : public QDialog
{
    Q_OBJECT

public:
    static bool otevrit(Uzivatel *&prihlasenyUzivatel, QWidget *rodic);

private slots:
    void on_vstupUzivatel_textChanged(const QString &text);
    void on_tlacitkoZrusit_clicked();
    void on_tlacitkoPrihlasit_clicked();

private:
    explicit DialogPrihlaseni(Uzivatel *&prihlasenyUzivatel, QWidget *rodic = 0);
    ~DialogPrihlaseni();

    void keyPressEvent(QKeyEvent *udalost) override;

    Uzivatel *&prihlasenyUzivatel_;

    Ui::DialogPrihlaseni *ui;
    ValidatorVstupu *validator_;
};

#endif // DIALOGPRIHLASENI_H
