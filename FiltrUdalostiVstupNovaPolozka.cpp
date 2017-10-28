#include "FiltrUdalostiVstupNovaPolozka.h"

FiltrUdalostiVstupNovaPolozka::FiltrUdalostiVstupNovaPolozka(QObject *rodic) : QObject(rodic) {
    connect(this, SIGNAL(signalEnterStisknuto()),
            rodic, SLOT(on_tlacitkoPridatPolozku_clicked()));

    connect(this, SIGNAL(signalCtrlEnterStisknuto()),
            rodic, SLOT(on_tlacitkoVytvoritPruvodku_clicked()));

    connect(this, SIGNAL(signalCtrlDelStisknuto()),
            rodic, SLOT(odstranitPosledniPolozku()));
}

bool FiltrUdalostiVstupNovaPolozka::eventFilter(QObject *objekt, QEvent *udalost) {
    if (udalost->type() == QEvent::KeyPress) {
        QKeyEvent *stiskKlaves = static_cast<QKeyEvent*>(udalost);
        // Ctrl+Enter uzavre zavoz a ulozi pruvodku
        if ((stiskKlaves->key() == Qt::Key_Enter || stiskKlaves->key() == Qt::Key_Return)
                && (stiskKlaves->modifiers().testFlag(Qt::ControlModifier))) {
            emit signalCtrlEnterStisknuto();
            return true;
        }
        // samotny Enter vlozi polozku do seznamu
        else if (stiskKlaves->key() == Qt::Key_Enter || stiskKlaves->key() == Qt::Key_Return) {
            emit signalEnterStisknuto();
            return true;
        }
        // Ctrl+Delete smaze aktualni polozku
        else if ((stiskKlaves->key() == Qt::Key_Delete)
                 && stiskKlaves->modifiers().testFlag(Qt::ControlModifier)) {
            emit signalCtrlDelStisknuto();
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return QObject::eventFilter(objekt, udalost);
    }
}
