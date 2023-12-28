#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QInputDialog>
#include <QFile>
#include <QStringListModel>
#include <QListView>
#include <QString>

#include "dbhelper.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator translator;

    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "BibleMate_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    //Create database if not exist for bibles and notes

    //Mere the bible selected



    MainWindow w;
    w.show();

    return app.exec();
}
