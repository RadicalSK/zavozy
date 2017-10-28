#ifndef NASEPTAVAC_H
#define NASEPTAVAC_H

#include <QCompleter>
#include <QFile>
#include <QStringListModel>
#include <QTextStream>
#include "Databaze.h"
#include "globalni.h"

class Naseptavac
{
public:
    Naseptavac();
    ~Naseptavac();

    QCompleter *naseptavacPrijmeni();
    QCompleter *naseptavacJmen(QString prijmeni);
    QCompleter *naseptavacObci();

private:
    QCompleter *naseptavacPrijmeni_;
    QCompleter *naseptavacJmen_;
    QCompleter *naseptavacObci_;

    QStringListModel *modelPrijmeni_;
    QStringListModel *modelJmen_;
    QStringListModel *modelObci_;
};

#endif // NASEPTAVAC_H
