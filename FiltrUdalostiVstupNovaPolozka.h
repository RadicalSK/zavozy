#ifndef FILTRUDALOSTIVSTUPNOVAPOLOZKA_H
#define FILTRUDALOSTIVSTUPNOVAPOLOZKA_H

#include <QKeyEvent>
#include <QObject>

class FiltrUdalostiVstupNovaPolozka : public QObject
{
    Q_OBJECT
public:
    explicit FiltrUdalostiVstupNovaPolozka(QObject *rodic = nullptr);

    bool eventFilter(QObject *objekt, QEvent *udalost);

signals:
    void signalEnterStisknuto();
    void signalCtrlEnterStisknuto();
    void signalCtrlDelStisknuto();
};

#endif // FILTRUDALOSTIVSTUPNOVAPOLOZKA_H
