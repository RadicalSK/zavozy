#include "Databaze.h"

bool Databaze::prihlasitKDatabazi(QString souborDb) {
    QSqlDatabase db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName(souborDb);

    QSqlQuery qry;

    if (db_.open()) {
        qry.exec("PRAGMA foreign_keys = ON");

        // otestuje existenci vsech pozadovanych tabulek
        bool tabulkyExistuji = true;
        foreach (QString tabulka, tabulkyDb_.keys()) {
            tabulkyExistuji &= qry.exec("SELECT * FROM " + tabulka);
        }

        // jestli neexistuji, zalozi se
        if (!tabulkyExistuji) {
            foreach (auto dotaz, tabulkyDb_) {
                qry.exec(dotaz);
            }
        }

        return true;
    }

    return false;
}

void Databaze::odhlasitOdDatabaze() {
    QSqlDatabase::database().close();
    QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());
}

int Databaze::pocetAktivnichUzivatelu() {
    QSqlQuery qry;
    qry.exec("SELECT COUNT(*) FROM uzivatel");
    if (qry.next()) {
        return qry.value(0).toInt();
    }

    return 0;
}

bool Databaze::vyhledatAktivnihoUzivatele(QString uzivJmeno, QString heslo, Uzivatel *&uzivatel) {
    QSqlQuery qry;
    qry.prepare("SELECT uzivJmeno, heslo, celeJmeno, admin FROM uzivatel "
                "WHERE uzivJmeno = :uzivJmeno AND heslo = :hesloSha256Hash AND aktivni = 1");
    qry.bindValue(":uzivJmeno", uzivJmeno);
    qry.bindValue(":hesloSha256Hash", Heslo::sha256Hash(heslo));
    qry.exec();
    if (qry.next()) {
        uzivatel = new Uzivatel(qry.value(0).toString(),
                                qry.value(1).toString(),
                                qry.value(2).toString(),
                                qry.value(3).toBool());
        return true;
    }

    return false;
}

bool Databaze::existujeUzivatel(QString uzivJmeno) {
    QSqlQuery qry;
    qry.prepare("SELECT * FROM uzivatel WHERE uzivJmeno = (:uzivJmeno)");
    qry.bindValue(":uzivJmeno", uzivJmeno);
    qry.exec();

    return qry.next();
}

bool Databaze::existujeUzivatel(QString uzivJmeno, bool &jeAktivni) {
    QSqlQuery qry;
    qry.prepare("SELECT uzivJmeno, aktivni FROM uzivatel WHERE uzivJmeno = :uzivJmeno");
    qry.bindValue(":uzivJmeno", uzivJmeno);
    qry.exec();
    if (qry.next()) {
        jeAktivni = qry.value(1).toBool();
        return true;
    }

    return false;
}

bool Databaze::zalozitNovehoUzivatele(QString uzivJmeno, QString heslo, QString celeJmeno, bool admin) {
    QSqlQuery qry;
    qry.prepare("INSERT INTO uzivatel VALUES (:uzivJmeno, :hesloSha256Hash, :celeJmeno, :admin, 1)");
    qry.bindValue(":uzivJmeno", uzivJmeno);
    qry.bindValue(":hesloSha256Hash", Heslo::sha256Hash(heslo));
    qry.bindValue(":celeJmeno", celeJmeno);
    qry.bindValue(":admin", admin ? 1 : 0);
    return qry.exec();
}

QStringList Databaze::seznamUzivatelu(bool pouzeAktivni) {
    QStringList seznamUzivatelu;
    QSqlQuery qry;
    qry.prepare("SELECT uzivJmeno, aktivni FROM uzivatel ORDER BY uzivJmeno ASC");
    qry.exec();
    while (qry.next()) {
        if (pouzeAktivni) {
            if (qry.value(1).toBool()) {
                seznamUzivatelu << qry.value(0).toString();
            }
            else
                continue;
        }
        else {
            QString uzivatel = qry.value(0).toString();
            if (!qry.value(1).toBool())
                uzivatel += " (neaktivní)";
            seznamUzivatelu << uzivatel;
        }
    }
    return seznamUzivatelu;
}

void Databaze::nacistUdajeUzivatele(QString uzivJmeno, QString &celeJmeno, bool &admin) {
    QSqlQuery qry;
    qry.prepare("SELECT celeJmeno, admin FROM uzivatel WHERE uzivJmeno = (:uzivJmeno)");
    qry.bindValue(":uzivJmeno", uzivJmeno);
    qry.exec();
    if (qry.next()) {
        celeJmeno = qry.value(0).toString();
        admin = qry.value(1).toBool();
    }
}

bool Databaze::zmenitHesloUzivatele(QString uzivJmeno, QString noveHeslo) {
    QSqlQuery qry;
    qry.prepare("UPDATE uzivatel SET heslo = :noveHesloHash WHERE uzivJmeno = :uzivJmeno");
    qry.bindValue(":uzivJmeno", uzivJmeno);
    qry.bindValue(":noveHesloHash", Heslo::sha256Hash(noveHeslo));
    return qry.exec();
}

bool Databaze::smazatUzivatele(QString uzivJmeno) {
    QSqlQuery qry;
    qry.prepare("UPDATE uzivatel SET aktivni = 0 WHERE uzivJmeno = :uzivJmeno");
    qry.bindValue(":uzivJmeno", uzivJmeno);
    return qry.exec();
}

bool Databaze::upravitUzivatele(QString uzivJmeno, QString noveCeleJmeno, bool noveAdmin) {
    QSqlQuery qry;
    qry.prepare("UPDATE uzivatel SET celeJmeno = :celeJmeno, admin = :admin WHERE uzivJmeno = :uzivJmeno");
    qry.bindValue(":celeJmeno", noveCeleJmeno);
    qry.bindValue(":admin", noveAdmin ? 1 : 0);
    qry.bindValue(":uzivJmeno", uzivJmeno);
    return qry.exec();
}

bool Databaze::verzeCiselniku(QString &verze, QDate &datumAktualizace, QString &uzivatel) {
    QSqlQuery qry;
    qry.prepare("SELECT verze, datum, celeJmeno FROM verzeCiselniku JOIN uzivatel "
                "ON verzeCiselniku.aktualizoval = uzivatel.uzivJmeno ORDER BY verze DESC LIMIT 1");
    qry.exec();
    if (qry.next()) {
        verze = qry.value(0).toString();
        datumAktualizace = QDate::fromString(qry.value(1).toString(), Qt::ISODate);
        uzivatel = qry.value(2).toString();
        return true;
    }

    return false;
}

bool Databaze::ulozitPomuckyDoCiselniku(QStringList &seznamHodnotKUlozeni) {
    int pocetHodnot = seznamHodnotKUlozeni.size();

    if ((pocetHodnot % 3) != 0)    // pocet hodnot musi byt delitelny 3
        return false;

    QString dotaz = "INSERT OR IGNORE INTO pomucka VALUES ('0', 'jiná položka než pomůcka', '-'), ('";
    for (int i = 0; i < pocetHodnot; i=i+3) {
        dotaz += QString(seznamHodnotKUlozeni[i] + "', '"
                         + seznamHodnotKUlozeni[i+1].replace("'", "''") + "', '"   // replace k sanitizaci SQL vstupu
                + seznamHodnotKUlozeni[i+2].replace("'", "''") + "'), ('");   // replace k sanitizaci SQL vstupu
    }

    // odstrani ", ('" na konci retezce
    dotaz.chop(4);

    QSqlQuery qry;
    return qry.exec(dotaz);
}

int Databaze::pocetPolozekZmenenychPoslednimDotazem() {
    QSqlQuery qry;
    qry.exec("SELECT changes()");
    if (qry.next()) {
        return qry.value(0).toInt();
    }

    return 0;
}

bool Databaze::ulozitAktualniVerziCiselniku(QString kodVerze, QString uzivatel) {
    QSqlQuery qry;
    qry.prepare("INSERT INTO verzeCiselniku VALUES (:verze, CURRENT_DATE, :uzivatel)");
    qry.bindValue(":verze", kodVerze);
    qry.bindValue(":uzivatel", uzivatel);
    return qry.exec();
}

bool Databaze::zalozitNovehoPacienta(Pacient *pacient) {
    QSqlQuery qry;
    qry.prepare("INSERT INTO pacient (prijmeni, jmeno, ulice, obec, telefon1, poznTel1, telefon2, poznTel2, poznamka) "
                "VALUES (:prijmeni, :jmeno, :ulice, :obec, :telefon1, :poznTel1, :telefon2, :poznTel2, :poznamka)");
    qry.bindValue(":prijmeni", pacient->prijmeni());
    qry.bindValue(":jmeno", pacient->jmeno());
    qry.bindValue(":ulice", pacient->ulice());
    qry.bindValue(":obec", pacient->obec());
    qry.bindValue(":telefon1", pacient->telefon1());
    qry.bindValue(":poznTel1", pacient->poznTel1());
    qry.bindValue(":telefon2", pacient->telefon2());
    qry.bindValue(":poznTel2", pacient->poznTel2());
    qry.bindValue(":poznamka", pacient->poznamka());
    return qry.exec();
}

bool Databaze::vyhledatPacienta(QString prijmeni, QString jmeno, Pacient *&pacient) {
    QSqlQuery qry;
    qry.prepare("SELECT pacId, prijmeni, jmeno, ulice, obec, telefon1, poznTel1, telefon2, poznTel2, poznamka "
                "FROM pacient WHERE prijmeni = :prijmeni AND jmeno = :jmeno");
    qry.bindValue(":prijmeni", prijmeni);
    qry.bindValue(":jmeno", jmeno);
    qry.exec();

    if (qry.next()) {
        pacient = new Pacient(qry.value(0).toInt(),
                              qry.value(1).toString(),
                              qry.value(2).toString(),
                              qry.value(3).toString(),
                              qry.value(4).toString(),
                              qry.value(5).toString(),
                              qry.value(6).toString(),
                              qry.value(7).toString(),
                              qry.value(8).toString(),
                              qry.value(9).toString());
        return true;
    }

    return false;
}

bool Databaze::upravitStavajicihoPacienta(Pacient *pacient) {
    QSqlQuery qry;
    qry.prepare("UPDATE pacient SET prijmeni = :prijmeni, jmeno = :jmeno, ulice = :ulice, obec = :obec, "
                "telefon1 = :telefon1, poznTel1 = :poznTel1, telefon2 = :telefon2, poznTel2 = :poznTel2, "
                "poznamka = :poznamka WHERE pacId = :pacId");
    qry.bindValue(":pacId", pacient->id());
    qry.bindValue(":prijmeni", pacient->prijmeni());
    qry.bindValue(":jmeno", pacient->jmeno());
    qry.bindValue(":ulice", pacient->ulice());
    qry.bindValue(":obec", pacient->obec());
    qry.bindValue(":telefon1", pacient->telefon1());
    qry.bindValue(":poznTel1", pacient->poznTel1());
    qry.bindValue(":telefon2", pacient->telefon2());
    qry.bindValue(":poznTel2", pacient->poznTel2());
    qry.bindValue(":poznamka", pacient->poznamka());
    return qry.exec();
}

int Databaze::ziskatIdPacienta(QString prijmeni, QString jmeno) {
    QSqlQuery qry;
    qry.prepare("SELECT pacId FROM pacient WHERE prijmeni = :prijmeni AND jmeno = :jmeno");
    qry.bindValue(":prijmeni", prijmeni);
    qry.bindValue(":jmeno", jmeno);
    qry.exec();

    if (qry.next()) {
        return qry.value(0).toInt();
    }

    return 0;
}

QStringList Databaze::seznamPrijmeniPacientu() {
    QStringList seznamPrijmeni;
    QSqlQuery qry;
    qry.exec("SELECT DISTINCT prijmeni FROM pacient ORDER BY prijmeni ASC");
    while (qry.next()) {
        seznamPrijmeni << qry.value(0).toString();
    }
    return seznamPrijmeni;
}

QStringList Databaze::seznamJmenKPrijmeni(QString prijmeni) {
    QStringList seznamJmen;
    QSqlQuery qry;
    qry.prepare("SELECT jmeno FROM pacient WHERE prijmeni = :prijmeni ORDER BY jmeno ASC");
    qry.bindValue(":prijmeni", prijmeni);
    qry.exec();
    while (qry.next()) {
        seznamJmen << qry.value(0).toString();
    }
    return seznamJmen;
}

void Databaze::prehledPomucekOdebranychPacientem(int pacId, QStringList &kodyVzp, QStringList &nazvy, QStringList &doplnky) {
    QSqlQuery qry;
    qry.prepare("SELECT kodVzp, nazev, doplnek FROM pomucka WHERE kodVzp IN "
                "(SELECT DISTINCT kodVzp FROM zavoz JOIN polozkaZavozu USING (pacId, datumZavozu) WHERE pacId = :pacId) "
                "ORDER BY kodVzp ASC");
    qry.bindValue(":pacId", pacId);
    qry.exec();
    while (qry.next()) {
        kodyVzp << qry.value(0).toString();
        nazvy << qry.value(1).toString();
        doplnky << qry.value(2).toString();
    }
}

bool Databaze::vyhledatPomucku(QString kodVzp, QString &nazev, QString &doplnek) {
    QSqlQuery qry;
    qry.prepare("SELECT nazev, doplnek FROM pomucka WHERE kodVzp = :kodVzp");
    qry.bindValue(":kodVzp", kodVzp);
    qry.exec();
    if (qry.next()) {
        nazev = qry.value(0).toString();
        doplnek = qry.value(1).toString();
        return true;
    }

    return false;
}

bool Databaze::ulozitZavoz(QString uzivatel, QString adresa, int pacId, QDate datumZavozu) {
    QSqlQuery qry;
    qry.prepare("INSERT INTO zavoz VALUES (CURRENT_DATE, :uzivatel, :adresa, :pacId, :datum)");
    qry.bindValue(":uzivatel", uzivatel);
    qry.bindValue(":adresa", adresa);
    qry.bindValue(":pacId", pacId);
    qry.bindValue(":datum", datumZavozu.toString(Qt::ISODate));
    return qry.exec();
}

bool Databaze::ulozitPolozkuZavozu(int pocet, int kodVzp, int pacId, QDate datumZavozu) {
    QSqlQuery qry;
    qry.prepare("INSERT INTO polozkaZavozu VALUES (:pocet, :kodVzp, :pacId, :datum)");
    qry.bindValue(":pocet", pocet);
    qry.bindValue(":kodVzp", kodVzp);
    qry.bindValue(":pacId", pacId);
    qry.bindValue(":datum", datumZavozu.toString(Qt::ISODate));
    return qry.exec();
}

bool Databaze::prehledPacientu(QSqlQueryModel *&queryModel) {
    QSqlQuery qry;
    qry.prepare("SELECT pacId AS 'ID', prijmeni AS 'Příjmení', jmeno AS 'Jméno', ulice AS 'Ulice, číslo', "
                "obec AS 'PSČ, obec', telefon1 AS 'Telefon', poznTel1 AS 'Pozn. tel.', "
                "telefon2 AS 'Telefon2', poznTel2 AS 'Pozn. tel.2', poznamka AS 'Poznámka' "
                "FROM pacient ORDER BY pacId ASC");
    qry.exec();

    if (qry.next()) {
        queryModel->setQuery(qry);
        return true;
    }

    return false;
}

bool Databaze::prehledZavozuProPacienta(int pacId, QDate datumOd, QDate datumDo, QSqlQueryModel *&queryModel) {
    QSqlQuery qry;
    qry.prepare("SELECT datumZavozu AS 'Datum závozu', SUBSTR('0000000' || kodVzp, -7, 7) AS 'Kód VZP', "
                "nazev AS 'Název pomůcky', doplnek AS 'Doplněk názvu', pocet AS 'Počet', adresa AS 'Doručovací adresa', "
                "poridil AS 'Pořídil', datumVychystani AS 'Pořízeno dne' "
                "FROM zavoz JOIN polozkaZavozu USING (pacId, datumZavozu) JOIN pomucka USING (kodVzp) "
                "WHERE pacId = :pacId AND datumZavozu >= :datumOd AND datumZavozu <= :datumDo "
                "ORDER BY datumZavozu DESC, kodVzp ASC");
    qry.bindValue(":pacId", pacId);
    qry.bindValue(":datumOd", datumOd.toString(Qt::ISODate));
    qry.bindValue(":datumDo", datumDo.toString(Qt::ISODate));
    qry.exec();

    if (qry.next()) {
        queryModel->setQuery(qry);
        return true;
    }

    return false;
}

bool Databaze::prehledZavozuUzivatele(QString uzivJmeno, QDate datumOd, QDate datumDo, QSqlQueryModel *&queryModel) {
    QSqlQuery qry;
    qry.prepare("SELECT datumZavozu as 'Dátum závozu', SUBSTR('0000000' || kodVzp, -7, 7) AS 'Kód VZP', "
                "nazev AS 'Název pomůcky', doplnek AS 'Doplněk názvu', pocet as 'Počet', prijmeni as 'Příjmení', "
                "jmeno as 'Jméno', adresa as 'Adresa', poridil as 'Pořídil', datumVychystani as 'Pořízeno dne' "
                "FROM zavoz JOIN polozkaZavozu USING (pacId, datumZavozu) JOIN pacient USING (pacId) JOIN pomucka USING (kodVzp) "
                "WHERE poridil = :uzivJmeno AND datumZavozu >= :datumOd AND datumZavozu <= :datumDo "
                "ORDER BY datumZavozu DESC, prijmeni ASC");
    qry.bindValue(":uzivJmeno", uzivJmeno);
    qry.bindValue(":datumOd", datumOd.toString(Qt::ISODate));
    qry.bindValue(":datumDo", datumDo.toString(Qt::ISODate));
    qry.exec();

    if (qry.next()) {
        queryModel->setQuery(qry);
        return true;
    }

    return false;
}

bool Databaze::prehledZavozuPomucky(QString kodVzp, QDate datumOd, QDate datumDo, QSqlQueryModel *&queryModel) {
    QSqlQuery qry;
    qry.prepare("SELECT datumZavozu as 'Dátum závozu', pocet as 'Počet', prijmeni as 'Příjmení', "
                "jmeno as 'Jméno', adresa as 'Adresa', poridil as 'Pořídil', datumVychystani as 'Pořízeno dne' "
                "FROM zavoz JOIN polozkaZavozu USING (pacId, datumZavozu) JOIN pacient USING (pacId) "
                "WHERE kodVzp = :kodVzp AND datumZavozu >= :datumOd AND datumZavozu <= :datumDo "
                "ORDER BY datumZavozu DESC, prijmeni ASC");
    qry.bindValue(":kodVzp", kodVzp);
    qry.bindValue(":datumOd", datumOd.toString(Qt::ISODate));
    qry.bindValue(":datumDo", datumDo.toString(Qt::ISODate));
    qry.exec();

    if (qry.next()) {
        queryModel->setQuery(qry);
        return true;
    }

    return false;
}

bool Databaze::prehledZavozu(QDate datumOd, QDate datumDo, QSqlQueryModel *&queryModel) {
    QSqlQuery qry;
    qry.prepare("SELECT datumZavozu as 'Dátum závozu', prijmeni as 'Příjmení', jmeno as 'Jméno', "
                "adresa as 'Adresa', kodVzp as 'Kód VZP', nazev as 'Název', doplnek as 'Doplněk', "
                "pocet as 'Počet', poridil as 'Pořídil', datumVychystani as 'Pořízeno dne' "
                "FROM pacient JOIN zavoz USING (pacId) JOIN polozkaZavozu USING (pacId, datumZavozu) "
                "JOIN pomucka USING (kodVzp) WHERE datumZavozu >= :datumOd AND datumZavozu <= :datumDo "
                "ORDER BY datumZavozu DESC, prijmeni ASC, kodVzp ASC");
    qry.bindValue(":datumOd", datumOd.toString(Qt::ISODate));
    qry.bindValue(":datumDo", datumDo.toString(Qt::ISODate));
    qry.exec();

    if (qry.next()) {
        queryModel->setQuery(qry);
        return true;
    }

    return false;
}

const QMap<QString, QString> Databaze::tabulkyDb_ = {
    {"uzivatel", "CREATE TABLE uzivatel (uzivJmeno TEXT PRIMARY KEY, heslo TEXT NOT NULL, celeJmeno TEXT NOT NULL, admin INTEGER DEFAULT 0, aktivni INTEGER DEFAULT 1)"},
    {"pacient", "CREATE TABLE pacient (pacId INTEGER PRIMARY KEY, prijmeni TEXT NOT NULL, jmeno TEXT NOT NULL, ulice TEXT NOT NULL, obec TEXT NOT NULL, telefon1 TEXT, poznTel1 TEXT, telefon2 TEXT, poznTel2 TEXT, poznamka TEXT)"},
    {"pomucka", "CREATE TABLE pomucka (kodVzp INTEGER PRIMARY KEY, nazev TEXT NOT NULL, doplnek TEXT)"},
    {"zavoz", "CREATE TABLE zavoz (datumVychystani TEXT NOT NULL, poridil TEXT NOT NULL, adresa TEXT NOT NULL, pacId INTEGER REFERENCES pacient(pacId), datumZavozu TEXT, PRIMARY KEY (pacId, datumZavozu))"},
    {"polozkaZavozu", "CREATE TABLE polozkaZavozu (pocet INTEGER NOT NULL, kodVzp INTEGER, pacId INTEGER, datumZavozu TEXT, FOREIGN KEY (kodVzp) REFERENCES pomucka(kodVzp), FOREIGN KEY (pacId, datumZavozu) REFERENCES zavoz(pacId, datumZavozu))"},
    {"verzeCiselniku", "CREATE TABLE verzeCiselniku (verze TEXT PRIMARY KEY, datum TEXT NOT NULL, aktualizoval TEXT NOT NULL REFERENCES uzivatel(uzivJmeno))"}
};
