#-------------------------------------------------
#
# Project created by QtCreator 2017-06-22T21:01:34
#
#-------------------------------------------------

QT       += core gui sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Zavozy
TEMPLATE = app
RC_ICONS = src/zavozy.ico

SOURCES += main.cpp\
        MainWindow.cpp \
    DialogPrihlaseni.cpp \
    DialogNovyUzivatel.cpp \
    Uzivatel.cpp \
    DialogUpravitSmazatUzivatele.cpp \
    DialogPacient.cpp \
    AktualizatorCiselniku.cpp \
    Pacient.cpp \
    ValidatorVstupu.cpp \
    Naseptavac.cpp \
    TiskarnaPruvodek.cpp \
    Databaze.cpp \
    PolozkaZavozu.cpp \
    Zavoz.cpp \
    WidgetOdstranovaciTlacitko.cpp \
    WidgetVstupniPolePoctu.cpp \
    WidgetSubsekcePomuckySeznam.cpp \
    DialogSeznamPruvodek.cpp \
    globalni.cpp \
    TiskoveFonty.cpp \
    TiskoveRozvrzeniPruvodky.cpp \
    FiltrUdalostiVstupNovaPolozka.cpp \
    AbstraktniDialogPrehled.cpp \
    DialogPrehledZavozuProPacienta.cpp \
    DialogPrehledZavozuVychystanychUzivatelem.cpp \
    DialogPrehledZavozuJednotlivePomucky.cpp \
    DialogPrehledVsechZavozuZaObdobi.cpp \
    DialogPrehledPacientu.cpp

HEADERS  += MainWindow.h \
    DialogPrihlaseni.h \
    DialogNovyUzivatel.h \
    Uzivatel.h \
    DialogUpravitSmazatUzivatele.h \
    DialogPacient.h \
    AktualizatorCiselniku.h \
    Pacient.h \
    ValidatorVstupu.h \
    Naseptavac.h \
    TiskarnaPruvodek.h \
    Databaze.h \
    PolozkaZavozu.h \
    Zavoz.h \
    WidgetOdstranovaciTlacitko.h \
    WidgetVstupniPolePoctu.h \
    WidgetSubsekcePomuckySeznam.h \
    DialogSeznamPruvodek.h \
    globalni.h \
    TiskoveFonty.h \
    TiskoveRozvrzeniPruvodky.h \
    FiltrUdalostiVstupNovaPolozka.h \
    AbstraktniDialogPrehled.h \
    DialogPrehledZavozuProPacienta.h \
    DialogPrehledZavozuVychystanychUzivatelem.h \
    DialogPrehledZavozuJednotlivePomucky.h \
    DialogPrehledVsechZavozuZaObdobi.h \
    DialogPrehledPacientu.h

FORMS    += MainWindow.ui \
    DialogPrihlaseni.ui \
    DialogNovyUzivatel.ui \
    DialogUpravitSmazatUzivatele.ui \
    DialogPacient.ui \
    DialogSeznamPruvodek.ui \
    AbstraktniDialogPrehled.ui

soubory.path = $$OUT_PWD/data
soubory.files += src/prirucka.html \
                    src/zmeny.html \
                    src/psc.txt

INSTALLS += soubory
