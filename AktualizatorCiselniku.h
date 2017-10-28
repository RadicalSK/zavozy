#ifndef AKTUALIZATORCISELNIKU_H
#define AKTUALIZATORCISELNIKU_H

#include <QDate>
#include <QFileDialog>
#include <QObject>
#include <QRegularExpression>
#include <QString>
#include <QTextStream>
#include "Databaze.h"
#include "globalni.h"

class AktualizatorCiselniku : public QObject
{
    Q_OBJECT

public:
    AktualizatorCiselniku(QWidget *parent = 0);

    bool aktualizovatCiselnik(QString souborCiselniku, QString uzivatel);

private:
    bool overitAktualnostVerze(QString nazevSouboru);
    bool ziskatNticiHodnotZRadku(const QString radek, QStringList &ntice);
    QString retezecZIbm850NaIbm852(QString retezec);
    QChar znakZIbm850NaIbm852(QChar znak);

    const int pocetNticNaDotaz_ = 100;
    QString kodAktualniVerze_;
    QDate datumPosledniAktualizace_;
    QString naposledAktualizoval_;

signals:
    void aktualizaceSpustena();
    void zatimZpracovano(int procento);
    void aktualizaceDokoncena();
};

#endif // AKTUALIZATORCISELNIKU_H
