#ifndef TISKOVEFONTY_H
#define TISKOVEFONTY_H

#include <QFont>
#include <QMap>

class TiskoveFonty
{
public:
    enum Styl {
        Normalni, Popisek, Jmeno
    };

    static QFont font(Styl styl);

private:
    static void nastavitFonty();

    static bool fontyNastavene_;
    static QMap<Styl, QFont> fonty_;
};

#endif // TISKOVEFONTY_H
