#ifndef WIDGETVSTUPNIPOLEPOCTU_H
#define WIDGETVSTUPNIPOLEPOCTU_H

#include <QFocusEvent>
#include <QKeyEvent>
#include <QSpinBox>

class WidgetVstupniPolePoctu : public QSpinBox
{
    Q_OBJECT
public:
    WidgetVstupniPolePoctu(int cisloPolozky, bool jeNova, QWidget *rodic = Q_NULLPTR);

private slots:
    void hodnotaZmenena(int hodnota);
    void upozornitNaNulovouHodnotu();

private:
    void vybarvitPozadi(bool vybarvit);

    void focusInEvent(QFocusEvent *udalost) override;
    void keyPressEvent(QKeyEvent *udalost) override;

    int cisloPolozky_;
    bool jeOdstranitelna_;

signals:
    void signalHodnotaZmenena(bool zmeneno);
    void signalCtrlEnterStisknuto();
    void signalCtrlDelStisknuto(int cisloPolozky);
};

#endif // WIDGETVSTUPNIPOLEPOCTU_H
