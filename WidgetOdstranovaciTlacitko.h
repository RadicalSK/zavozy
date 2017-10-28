#ifndef WIDGETODSTRANOVACITLACITKO_H
#define WIDGETODSTRANOVACITLACITKO_H

#include <QPushButton>

class WidgetOdstranovaciTlacitko : public QPushButton
{
    Q_OBJECT

public:
    WidgetOdstranovaciTlacitko(int cisloRadku, QWidget *rodic = 0);

public slots:
    void kliknuto(bool kliknuto);

signals:
    void signalOdstranitRadek(int cisloRadku);

private:
    int cisloRadku_;
};

#endif // WIDGETODSTRANOVACITLACITKO_H
