#ifndef TISKARNAPRUVODEK_H
#define TISKARNAPRUVODEK_H

#include <QPainter>
#include <QPrinter>
#include <QTextDocument>
#include "TiskoveFonty.h"
#include "TiskoveRozvrzeniPruvodky.h"
#include "Zavoz.h"

class TiskarnaPruvodek
{
public:
    TiskarnaPruvodek(const QList<Zavoz> seznamPruvodek);

private:
    QString textRadkuPolozky(PolozkaZavozu polozka);

    QRect horniPulka_, dolniPulka_;
};

#endif // TISKARNAPRUVODEK_H
