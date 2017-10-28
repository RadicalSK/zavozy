#include <QApplication>
#include "globalni.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    Soubory::vytvoritAdresare();
    Soubory::odstranitNeaktualniSouborVytistene();
    Soubory::odstranitNadbytecneZalohyDb();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
