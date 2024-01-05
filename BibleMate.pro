QT += core gui \
      sql \
      printsupport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    book.cpp \
    dbhelper.cpp \
    listviewdelegate.cpp \
    listviewdelegate_2.cpp \
    listviewdelegate_4.cpp \
    main.cpp \
    mainwindow.cpp \
    note.cpp \
    verse.cpp

HEADERS += \
    book.h \
    dbhelper.h \
    listviewdelegate.h \
    listviewdelegate_2.h \
    listviewdelegate_4.h \
    mainwindow.h \
    note.h \
    verse.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    BibleMate_it_IT.ts \
    BibleMate_es_ES.ts \
    BibleMate_fr_FR.ts \
    BibleMate_ru_RU.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    BibleMate_it_IT.ts \
    BibleMate_es_ES.ts \
    BibleMate_fr_FR.ts \
    BibleMate_ru_RU.ts \
    bibles/cei2008.db \
    bibles/diodati.db \
    bibles/kingjamesversion.db \
    bibles/newinternationalversionus.db \
    bibles/newkingjamesversion.db \
    bibles/nuovadiodati.db \
    bibles/nuovariveduta.db \
    bibles/reinavalera.db \
    bibles/synodal.db \
    img/bible.png \
    img/bible2.png \
    img/exit.png \
    img/fontsize.png \
    img/ic_launcher.png \
    img/ita.png \
    img/note1.png \
    img/note2.png \
    img/note3.png \
    img/pen1.png \
    img/pen2.png \
    img/po.png \
    img/ru.png \
    img/search.png \
    img/sp.png \
    img/update.png \
    img/us.png

RESOURCES += \
    resources.qrc

