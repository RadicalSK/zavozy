#ifndef WIDGETSUBSEKCEPOMUCKYSEZNAM_H
#define WIDGETSUBSEKCEPOMUCKYSEZNAM_H

#include <QGridLayout>
#include <QLabel>
#include <QMap>
#include <QScrollArea>
#include <QSpacerItem>
#include <QSpinBox>
#include <QStringList>
#include "PolozkaZavozu.h"
#include "WidgetOdstranovaciTlacitko.h"
#include "WidgetVstupniPolePoctu.h"

class WidgetSubsekcePomuckySeznam : public QWidget
{
    Q_OBJECT

public:
    WidgetSubsekcePomuckySeznam(QWidget *rodic = 0);

    bool neniDuplicita(QString kodVzp);
    WidgetVstupniPolePoctu *vstupniPolePoctuPolozky(QString kodVzp);
    WidgetVstupniPolePoctu* aktualniVstupniPolePoctu();
    QList<PolozkaZavozu> zvolenePomucky();

public slots:
    void pridatRadek(QString kodVzp, QString nazev, QString doplnek, bool jeNova = false);
    void odstranitRadek(int cisloVlozenePolozky);
    void odstranitPosledniRadek();
    void vyprazdnit();

signals:
    void signalRadekOdstranen();

private:
    struct RadekMrizky {
        QLabel *popisekKoduVzp;
        QLabel *popisekNazvu;
        WidgetVstupniPolePoctu *vstupniPolePoctu;
        QLabel *popisekKs;
        QSpacerItem *vycpavka;
        WidgetOdstranovaciTlacitko *tlacitkoOdstraneni = nullptr;
    };

    RadekMrizky radekMrizky(QString kodVzp, QString nazev, QString doplnek, bool jeNova = false);
    void smazatPrvkyRadku(RadekMrizky radek);

    QGridLayout *mrizka_;
    QWidget *hlavniOkno_;
    QSpacerItem *vertikalniVycpavka_;

    int cisloVlozenePolozky_ = 0;

    QMap<int, int> pouziteKodyVzp_;
    QMap<int, RadekMrizky> radkyMrizky_;
};

#endif // WIDGETSUBSEKCEPOMUCKYSEZNAM_H
