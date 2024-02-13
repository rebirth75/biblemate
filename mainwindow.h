#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include "dbhelper.h"
#include "book.h"
#include "note.h"
#include <QMainWindow>
#include <QItemSelectionModel>
#include <QStringListModel>
#include <QSettings>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QTextDocument>
#include <QPrinter>
#include "worker.h"
#include "bible_version.h"
#include "QProgressBar"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_pushButton_clicked();

    void Update_tab1(const QModelIndex &current, const QModelIndex &previous);

    void Update_tab3(const QModelIndex &current, const QModelIndex &previous);

    void Update_tab4(const QModelIndex &current, const QModelIndex &previous);

    void Update_tab2(const QModelIndex &current, const QModelIndex &previous);

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_lineEdit_returnPressed();

    void on_pushButton_5_clicked();

    void on_actionAbout_triggered();

    void on_actionExit_triggered();

    void on_pushButton_4_clicked();

    void on_pushButton_6_clicked();

    void on_chapter_CBox_currentIndexChanged(int index);

    void on_pushButton_7_clicked();

    void on_book_CBox_currentIndexChanged(int index);

    void print_T1(QPrinter *p);
    void print_T3(QPrinter *p);

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_actionNotes_file_location_triggered();

    void on_pushButton_13_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_18_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_20_clicked();

    void on_pushButton_21_clicked();

    void on_pushButton_22_clicked();

    void on_pushButton_23_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_24_clicked();

    void executeWhenVisible();

    void on_actionImport_Osis_XML_Bible_triggered();

    void on_version_CBox_currentIndexChanged(int index);

    void on_select_clicked();
    void on_import_clicked();
    void on_cancel_clicked();
    void on_work_finished();

    void on_player_finished();

    void on_pushButton_25_clicked();

    void on_actionChange_font_triggered();

    void on_pushButton_26_clicked();

    void on_pushButton_27_clicked();

    void on_pushButton_29_clicked();

    void on_pushButton_28_clicked();

private:
    Ui::MainWindow          *ui;
    QStringListModel        *m_Model;
    QItemSelectionModel     *selectionModel;
//    DBHelper                *dbhelper;
    Book                    book;    
    std::vector<Bible_version> bible_versions;
    std::vector<Note>       allNotes_T1, allNotes_T3, allNotes_T4;
    std::vector<QString>    index;
    std::vector<Verse>      results;
    QStringList             books_name;

//    Note                    *selNote_T1, *selNote_T3;
//    int                     fontsize;
    QString                 version;
//    QSettings               settings;
    QString                 path_bibles, path_notes;
    QString                 lastribbon;
    void setComboBox(QString ref);
    void addImportedBibles();
    bool eventFilter(QObject *watched, QEvent *event);
    QThread * thread;
    Worker * worker;
    QProgressBar * pBar;
    QFont bibleFont;
};
#endif // MAINWINDOW_H
