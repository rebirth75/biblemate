QT       += core gui \
            sql \
            printsupport \
            multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bible_version.cpp \
    book.cpp \
    dbhelper.cpp \
    listviewdelegate.cpp \
    listviewdelegate_2.cpp \
    listviewdelegate_4.cpp \
    main.cpp \
    mainwindow.cpp \
    note.cpp \
    verse.cpp \
    worker.cpp

HEADERS += \
    bible_version.h \
    book.h \
    dbhelper.h \
    listviewdelegate.h \
    listviewdelegate_2.h \
    listviewdelegate_4.h \
    mainwindow.h \
    note.h \
    verse.h \
    worker.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    BibleMate_it_IT.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    bibles.qrc \
    resources.qrc

DISTFILES += \
    BibleMate_es_ES.ts \
    BibleMate_fr_FR.ts \
    BibleMate_ru_RU.ts
