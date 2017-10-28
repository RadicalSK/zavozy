#ifndef DATABAZE_H
#define DATABAZE_H

#include <QtSql>
#include "globalni.h"
#include "Pacient.h"
#include "Uzivatel.h"

class Databaze
{
public:
    static bool prihlasitKDatabazi(QString souborDb);
    static void odhlasitOdDatabaze();

    // uzivatel
    static int pocetAktivnichUzivatelu();
    static bool vyhledatAktivnihoUzivatele(QString uzivJmeno, QString heslo, Uzivatel *&uzivatel);
    static bool existujeUzivatel(QString uzivJmeno);
    static bool existujeUzivatel(QString uzivJmeno, bool &jeAktivni);
    static bool zalozitNovehoUzivatele(QString uzivJmeno, QString heslo, QString celeJmeno, bool admin);
    static QStringList seznamUzivatelu(bool pouzeAktivni = true);
    static void nacistUdajeUzivatele(QString uzivJmeno, QString &celeJmeno, bool &admin);
    static bool zmenitHesloUzivatele(QString uzivJmeno, QString noveHeslo);
    static bool smazatUzivatele(QString uzivJmeno);
    static bool upravitUzivatele(QString uzivJmeno, QString noveCeleJmeno, bool noveAdmin);

    // ciselnik
    static bool verzeCiselniku(QString &verze, QDate &datumAktualizace, QString &uzivatel);
    static bool ulozitPomuckyDoCiselniku(QStringList &seznamHodnotKUlozeni);
    static int pocetPolozekZmenenychPoslednimDotazem();
    static bool ulozitAktualniVerziCiselniku(QString kodVerze, QString uzivatel);

    // pacient
    static bool zalozitNovehoPacienta(Pacient *pacient);
    static bool vyhledatPacienta(QString prijmeni, QString jmeno, Pacient *&pacient);
    static bool upravitStavajicihoPacienta(Pacient *pacient);
    static QStringList seznamPrijmeniPacientu();
    static QStringList seznamJmenKPrijmeni(QString prijmeni);
    static int ziskatIdPacienta(QString prijmeni, QString jmeno);

    // pomucky
    static void prehledPomucekOdebranychPacientem(int pacId, QStringList &kodyVzp, QStringList &nazvy, QStringList &doplnky);
    static bool vyhledatPomucku(QString kodVzp, QString &nazev, QString &doplnek);

    // zavozy
    static bool ulozitZavoz(QString uzivatel, QString adresa, int pacId, QDate datumZavozu);
    static bool ulozitPolozkuZavozu(int pocet, int kodVzp, int pacId, QDate datumZavozu);

    // prehledy
    static bool prehledPacientu(QSqlQueryModel *&queryModel);
    static bool prehledZavozuProPacienta(int pacId, QDate datumOd, QDate datumDo, QSqlQueryModel *&queryModel);
    static bool prehledZavozuUzivatele(QString uzivJmeno, QDate datumOd, QDate datumDo, QSqlQueryModel *&queryModel);
    static bool prehledZavozuPomucky(QString kodVzp, QDate datumOd, QDate datumDo, QSqlQueryModel *&queryModel);
    static bool prehledZavozu(QDate datumOd, QDate datumDo, QSqlQueryModel *&queryModel);

private:
    static const QMap<QString, QString> tabulkyDb_;
};

#endif // DATABAZE_H
