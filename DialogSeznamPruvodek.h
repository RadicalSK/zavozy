#ifndef DIALOGSEZNAMPRUVODEK_H
#define DIALOGSEZNAMPRUVODEK_H

#include <QDialog>
#include <QKeyEvent>
#include <QListWidget>
#include "globalni.h"
#include "Zavoz.h"

namespace Ui {
class DialogSeznamPruvodek;
}

class DialogSeznamPruvodek : public QDialog
{
    Q_OBJECT

public:
    static bool otevritNevytistene(QList<Zavoz> *nevytistenePruvodky);
    static bool otevritOpetovneZarazeniDoFronty(QList<Zavoz> *dnesVytistenePruvodky, QList<Zavoz> *nevytistenePruvodky);

private slots:
    void on_seznamDnesVytistene_currentRowChanged(int radek);
    void on_seznamDnesVytistene_activated(const QModelIndex &index);
    void on_tlacitkoZaraditDoFronty_clicked();
    void on_tlacitkoZrusit_clicked();
    void on_tlacitkoPotvrdit_clicked();

private:
    explicit DialogSeznamPruvodek(QList<Zavoz> *dnesVytistenePruvodky, QList<Zavoz> *nevytistenePruvodky, QWidget *rodic = 0);
    ~DialogSeznamPruvodek();

    void keyPressEvent(QKeyEvent *udalost) override;

    bool naplnitSeznamPruvodek(QList<Zavoz> *seznamPruvodek, QListWidget *seznam);

    bool provedeneZmeny_ = false;
    int aktivniRadek_ = -1;
    QList<Zavoz> *p_dnesVytistenePruvodky_;
    QList<Zavoz> *p_nevytistenePruvodky_;
    QList<Zavoz> obnovenePruvodky_;

    Ui::DialogSeznamPruvodek *ui;
};

#endif // DIALOGSEZNAMPRUVODEK_H
