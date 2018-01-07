#include "DialogPrehledZavozuJednotlivePomucky.h"
#include "ui_AbstraktniDialogPrehled.h"

DialogPrehledZavozuJednotlivePomucky::DialogPrehledZavozuJednotlivePomucky(QWidget *rodic)
    : AbstraktniDialogPrehled(rodic),
      stalyPopisekKodVzp_(new QLabel("Kód VZP:")), vstupKodVzp_(new QLineEdit),
      tlacitkoNajitVCiselniku_(new QPushButton("Najít v číselníku")), popisekPomucka_(new QLabel)
{
    // prizbusobi texty
    this->setWindowTitle("Přehled závozů jednotlivé pomůcky");
    ui->sekceVyhledavaciKriterium->setTitle("Pomůcka:");

    // prida potrebne widgety do rozvrzeni
    QLayout *rozvrzeniSekce = ui->sekceVyhledavaciKriterium->layout();
    rozvrzeniSekce->addWidget(stalyPopisekKodVzp_);
    rozvrzeniSekce->addWidget(vstupKodVzp_);
    rozvrzeniSekce->addWidget(tlacitkoNajitVCiselniku_);
    rozvrzeniSekce->addWidget(popisekPomucka_);

    // nastavi widgety
    vstupKodVzp_->setValidator(new QIntValidator(0, 9999999, this));
    vstupKodVzp_->setMaximumWidth(100);
    vstupKodVzp_->setFocus();
    tlacitkoNajitVCiselniku_->setEnabled(false);
    tlacitkoNajitVCiselniku_->setMaximumWidth(100);
    popisekPomucka_->setStyleSheet("font-weight: bold");
    popisekPomucka_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // navaze udalosti
    connect(vstupKodVzp_, &QLineEdit::textChanged,
            this, &DialogPrehledZavozuJednotlivePomucky::zpristupnitVyhledavaciTlacitko);

    connect(vstupKodVzp_, &QLineEdit::returnPressed,
            tlacitkoNajitVCiselniku_, &QPushButton::click);

    connect(tlacitkoNajitVCiselniku_, &QPushButton::clicked,
            this, &DialogPrehledZavozuJednotlivePomucky::vyhledatPomuckuVDatabazi);
}

void DialogPrehledZavozuJednotlivePomucky::otevrit(QWidget *rodic) {
    DialogPrehledZavozuJednotlivePomucky *dialog = new DialogPrehledZavozuJednotlivePomucky(rodic);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->showMaximized();
    dialog->exec();
}

void DialogPrehledZavozuJednotlivePomucky::zpristupnitVyhledavaciTlacitko(QString zadanyText) {
    tlacitkoNajitVCiselniku_->setEnabled(!zadanyText.isEmpty());
    tlacitkoNajitVCiselniku_->setVisible(true);
    popisekPomucka_->clear();
    zpristupnitTabulku(false);
    povolitVyhledavaciTlacitko(false);
}

void DialogPrehledZavozuJednotlivePomucky::vyhledatPomuckuVDatabazi() {
    QString nazev, doplnek;

    if (Databaze::vyhledatPomucku(vstupKodVzp_->text(), nazev, doplnek)
        && (vstupKodVzp_->text().toInt() != 0)) {
        QString zobrazenyNazev = nazev;
        if (!doplnek.isEmpty())
            zobrazenyNazev += QString("<br>" + doplnek);
        popisekPomucka_->setText(zobrazenyNazev);
        tlacitkoNajitVCiselniku_->setVisible(false);
        povolitVyhledavaciTlacitko(true);
    }
    else {
        validator_->zobrazitUpozorneni(QString("Číselník neobsahuje položku s kódem " + vstupKodVzp_->text()));
    }

    vstupKodVzp_->selectAll();
}

bool DialogPrehledZavozuJednotlivePomucky::vyhledatZaznamy() {
    return Databaze::prehledZavozuPomucky(vstupKodVzp_->text(), datumOd_, datumDo_, queryModel_);
}

QString DialogPrehledZavozuJednotlivePomucky::navrhnoutNazevCsvSouboru() {
    return QString("závozy pomůcky %1 za období %2.csv").arg(vstupKodVzp_->text())
            .arg(AbstraktniDialogPrehled::casoveRozpetiProNazev());
}
