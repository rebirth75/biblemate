#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dbhelper.h"
#include "note.h"
#include <QMainWindow>
#include <QItemSelectionModel>
#include <QStringListModel>
#include <QSettings>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QTextDocument>
#include <QPrinter>

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

    void on_actionNuova_Diodati_triggered();

    void on_actionNew_International_triggered();

    void on_action22_triggered();

    void on_action20_triggered();

    void on_action18_triggered();

    void on_action16_triggered();

    void on_action14_triggered();

    void on_actionAbout_triggered();

    void on_actionKing_James_triggered();

    void on_actionNew_King_James_triggered();

    void on_actionCEI_2008_triggered();

    void on_actionNuova_Riveduta_triggered();

    void on_actionDiodati_triggered();

    void on_actionSynodal_triggered();

    void on_actionExit_triggered();

    void on_pushButton_4_clicked();

    void on_pushButton_6_clicked();

    void on_chapter_CBox_currentTextChanged(const QString &arg1);

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

private:
    Ui::MainWindow          *ui;
    QStringListModel        *m_Model;
    QItemSelectionModel     *selectionModel;
    DBHelper                *dbhelper;
    Book                    book;

    std::vector<Note>       allNotes_T1, allNotes_T3, allNotes_T4;
    std::vector<QString>    index;
    std::vector<Verse>      results;
    QStringList             books_name;
    QStringList             chapters, verses;
    Note                    *selNote_T1, *selNote_T3;
    int                     fontsize;
    QString                 version;
    QSettings               *settings;
    QString                 path_bibles, path_notes;
    QString                 lastribbon;
    void setComboBox(QString ref);
    bool eventFilter(QObject *watched, QEvent *event);

};
#endif // MAINWINDOW_H
