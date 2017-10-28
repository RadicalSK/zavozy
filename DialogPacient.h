#ifndef DIALOGPACIENT_H
#define DIALOGPACIENT_H

#include <QComboBox>
#include <QCompleter>
#include <QDialog>
#include <QKeyEvent>
#include <QLineEdit>
#include "Databaze.h"
#include "globalni.h"
#include "Naseptavac.h"
#include "Pacient.h"
#include "ValidatorVstupu.h"

namespace Ui {
class DialogPacient;
}

class DialogPacient : public QDialog
{
    Q_OBJECT

public:
    enum TypDialogu {
        NovyPacient,
        UpravaPacienta
    };

    static bool otevritNovyPacient();
    static bool otevritNovyPacient(QString prijmeni, QString jmeno);
    static bool otevritUpravaPacienta();
    static bool otevritUpravaPacienta(Pacient *upravovanyPacient);

private slots:
    void on_vstupPrijmeni_textChanged(const QString &text);
    void on_vstupPrijmeni_editingFinished();
    void on_vstupPrijmeni_returnPressed();
    void on_vstupJmeno_editingFinished();
    void on_vstupJmeno_returnPressed();
    void on_volbaUpravitJmeno_clicked(bool zvoleno);
    void on_vstupUlice_editingFinished();
    void on_vstupObec_editingFinished();
    void on_vstupTelefon_editingFinished();
    void on_volbaNemaTelefon_toggled(bool zvoleno);
    void on_vstupPoznTel_editingFinished();
    void on_vstupTelefon2_editingFinished();
    void on_vstupPoznTel2_editingFinished();

    void on_tlacitkoZrusit_clicked();
    void on_tlacitkoUlozit_clicked();

private:
    explicit DialogPacient(TypDialogu typ, Pacient *upravovanyPacient = nullptr, QWidget *rodic = nullptr);
    explicit DialogPacient(QString prijmeni, QString jmeno, QWidget *rodic = nullptr);
    explicit DialogPacient(Pacient *upravovanyPacient, QWidget *rodic = nullptr);
    ~DialogPacient();

    // metody pro manipulaci s GUI
    void pripravitVyhledavaniPacienta();
    void nacistPacientaKUprave();

    void zpristupnitVstupyPrijmeniAJmeno(bool zpristupnit);
    void zpristupnitOstatniVstupy(bool zpristupnit);
    void zpristupnitVolbuNemaTelefon(bool zpristupnit);
    void zpristupnitVstupyTelefonu(bool zpristupnit);

    void zobrazitIdPacienta(Pacient *pacient, bool zobrazit = true);
    void naplnitVstupyUdaji(Pacient *pacient);

    // validacni metody
    bool validovatVstupPrijmeni();
    bool validovatVstupJmeno();
    bool validovatVstupUlice();
    bool validovatVstupObec();
    bool validovatVstupTelefon();
    bool validovatVstupPoznTel();
    bool validovatVstupTelefon2();
    bool validovatVstupPoznTel2();
    bool prijmeniNeboJmenoZmeneno();

    // metody komunikujici s databazi
    void vyhledatPacientaKUprave();
    bool existujePacientSeStejnymJmenem();
    void zalozitNovehoPacienta();
    void upravitStavajicihoPacienta();

    // ostatni metody
    void chybovaHlaskaDuplicita(Pacient *jmenovec);
    void keyPressEvent(QKeyEvent *udalost) override;

    Pacient *aktualniPacient_;
    Pacient *pacient_ = nullptr;    // novy pacient nebo pracovni kopie stavajiciho pacienta

    TypDialogu typDialogu_;
    Naseptavac *naseptavac_;
    Ui::DialogPacient *ui;
    ValidatorVstupu *validator_;

    bool rezimVyhledavaniPacienta_ = false;

    QList<QLineEdit*> vstupyPrijmeniAJmeno_;
    QList<QLineEdit*> ostatniVstupy_;
    QList<QLineEdit*> povinneVstupy_;
    QList<QLineEdit*> vstupyTelefonu_;
};

#endif // DIALOGPACIENT_H
