#include "Naseptavac.h"

Naseptavac::Naseptavac() :
    naseptavacPrijmeni_(new QCompleter), naseptavacJmen_(new QCompleter), naseptavacObci_(new QCompleter),
    modelPrijmeni_(new QStringListModel), modelJmen_(new QStringListModel), modelObci_(new QStringListModel)
{}

Naseptavac::~Naseptavac() {
    delete naseptavacPrijmeni_;
    delete naseptavacJmen_;
    delete naseptavacObci_;

    delete modelPrijmeni_;
    delete modelJmen_;
    delete modelObci_;
}

QCompleter *Naseptavac::naseptavacPrijmeni() {
    QStringList seznamPrijmeni = Databaze::seznamPrijmeniPacientu();

    modelPrijmeni_->setStringList(seznamPrijmeni);

    naseptavacPrijmeni_->setModel(modelPrijmeni_);
    naseptavacPrijmeni_->setCaseSensitivity(Qt::CaseInsensitive);

    return naseptavacPrijmeni_;
}

QCompleter *Naseptavac::naseptavacJmen(QString prijmeni) {
    QStringList seznamJmen = Databaze::seznamJmenKPrijmeni(prijmeni);

    modelJmen_->setStringList(seznamJmen);

    naseptavacJmen_->setModel(modelJmen_);
    naseptavacJmen_->setCaseSensitivity(Qt::CaseInsensitive);

    return naseptavacJmen_;
}

QCompleter *Naseptavac::naseptavacObci() {
    QStringList seznamObci;

    QFile souborObce(Soubory::psc);
    if (souborObce.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&souborObce);
        in.setCodec("UTF-8");
        while (!souborObce.atEnd()) {
            seznamObci << in.readLine();
        }

        souborObce.close();
    }
    else {
        OknoZprav::upozorneni("Soubor se seznamem PSČ " + Text::kurziva(Soubory::psc) + " se nepodařilo otevřít:",
                              souborObce.errorString());
    }

    modelObci_->setStringList(seznamObci);

    naseptavacObci_->setModel(modelObci_);
    naseptavacObci_->setCaseSensitivity(Qt::CaseInsensitive);

    return naseptavacObci_;
}
