#include "AbstraktniDialogPrehled.h"
#include "ui_AbstraktniDialogPrehled.h"

AbstraktniDialogPrehled::AbstraktniDialogPrehled(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AbstraktniDialogPrehled)
{
    ui->setupUi(this);
}

AbstraktniDialogPrehled::~AbstraktniDialogPrehled()
{
    delete ui;
}
