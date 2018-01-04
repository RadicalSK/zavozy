#ifndef ABSTRAKTNIDIALOGPREHLED_H
#define ABSTRAKTNIDIALOGPREHLED_H

#include <QDialog>

namespace Ui {
    class AbstraktniDialogPrehled;
}

class AbstraktniDialogPrehled : public QDialog
{
    Q_OBJECT

public:
    explicit AbstraktniDialogPrehled(QWidget *parent = 0);
    ~AbstraktniDialogPrehled();

private:
    Ui::AbstraktniDialogPrehled *ui;
};

#endif // ABSTRAKTNIDIALOGPREHLED_H
