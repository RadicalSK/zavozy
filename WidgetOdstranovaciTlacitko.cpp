#include "WidgetOdstranovaciTlacitko.h"

WidgetOdstranovaciTlacitko::WidgetOdstranovaciTlacitko(int cisloRadku, QWidget *rodic)
    : QPushButton(rodic), cisloRadku_(cisloRadku)
{
    this->setText("X");
    this->adjustSize();
    this->setFixedWidth(this->size().height());
    this->setStyleSheet("QPushButton {color: red}");
    this->setToolTip("Odstranit položku ze seznamu (Ctrl+Del)");

    connect(this, &WidgetOdstranovaciTlacitko::clicked,
            this, &WidgetOdstranovaciTlacitko::kliknuto);
}

void WidgetOdstranovaciTlacitko::kliknuto(bool kliknuto) {
    Q_UNUSED(kliknuto)
    emit signalOdstranitRadek(cisloRadku_);
}
