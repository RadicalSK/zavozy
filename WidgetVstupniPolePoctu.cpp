#include "WidgetVstupniPolePoctu.h"

WidgetVstupniPolePoctu::WidgetVstupniPolePoctu(int cisloPolozky, bool jeNova, QWidget *rodic)
    : QSpinBox(rodic), cisloPolozky_(cisloPolozky), jeOdstranitelna_(jeNova)
{
    this->clear();
    this->adjustSize();
    this->setMaximum(999);
    this->setFixedWidth(this->width());

    // signalizovat zmenu hodnoty
    connect(this, static_cast<void (QSpinBox::*)(int)>(&WidgetVstupniPolePoctu::valueChanged),
            this, &WidgetVstupniPolePoctu::hodnotaZmenena);

    // signalizovat nulovou hodnotu
    connect(this, &WidgetVstupniPolePoctu::editingFinished,
            this, &WidgetVstupniPolePoctu::upozornitNaNulovouHodnotu);
}

void WidgetVstupniPolePoctu::hodnotaZmenena(int hodnota) {
    if (hodnota) {
        emit signalHodnotaZmenena(true);
    }
}

void WidgetVstupniPolePoctu::upozornitNaNulovouHodnotu() {
    if (!this->value()) {
        vybarvitPozadi(true);
    }
}

void WidgetVstupniPolePoctu::vybarvitPozadi(bool vybarvit) {
    vybarvit ? this->setStyleSheet("background-color: salmon")
             : this->setStyleSheet("");
}

void WidgetVstupniPolePoctu::focusInEvent(QFocusEvent *udalost) {
    vybarvitPozadi(false);
    return QSpinBox::focusInEvent(udalost);
}

void WidgetVstupniPolePoctu::keyPressEvent(QKeyEvent *udalost) {
    // Ctrl+Enter vysle signal na vytvoreni pruvodky
    if ((udalost->key() == Qt::Key_Enter || udalost->key() == Qt::Key_Return)
            && (udalost->modifiers().testFlag(Qt::ControlModifier))) {
        emit signalCtrlEnterStisknuto();
    }
    // Enter posune fokus stejne jako Tab
    else if (udalost->key() == Qt::Key_Enter || udalost->key() == Qt::Key_Return) {
        QKeyEvent novaUdalost(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
        QSpinBox::keyPressEvent(&novaUdalost);
        udalost->setAccepted(novaUdalost.isAccepted());
    }
    // Ctrl+Del vysle signal na smazani radku
    else if ((udalost->key() == Qt::Key_Delete)
             && udalost->modifiers().testFlag(Qt::ControlModifier)
             && jeOdstranitelna_) {
        emit signalCtrlDelStisknuto(cisloPolozky_);
    }
    else {
        QSpinBox::keyPressEvent(udalost);
    }
}
