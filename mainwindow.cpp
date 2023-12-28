#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "book.h"
#include <QFile>
#include "dbhelper.h"
#include <QDebug>
#include <QStringListModel>
#include "listviewdelegate.h"
#include "listviewdelegate_2.h"
#include "listviewdelegate_4.h"
#include <QStandardItemModel>
#include <QDateTime>
#include <QStringRef>
#include <QRadioButton>
#include <QSettings>
#include <QStandardPaths>
#include <QFileDialog>
#include <QInputDialog>

QDateTime date = QDateTime::currentDateTime();
QString today = date.toString("yyyy.MM.dd");

QStringList books_id = {
    "Gen","Exod","Lev","Num","Deut","Josh","Judg","Ruth","1Sam","2Sam",
    "1Kgs","2Kgs","1Chr","2Chr","Ezra","Neh","Esth","Job","Ps","Prov",
    "Eccl","Song","Isa","Jer","Lam","Ezek","Dan","Hos","Joel","Amos",
    "Obad","Jonah","Mic","Nah","Hab","Zeph","Hag","Zech","Mal",
    "Matt","Mark","Luke","John","Acts","Rom","1Cor","2Cor","Gal",
    "Eph","Phil","Col","1Thess","2Thess","1Tim","2Tim","Titus","Phlm",
    "Heb","Jas","1Pet","2Pet","1John","2John","3John","Jude","Rev"};


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->plainTextEdit->installEventFilter(this);

    books_name = QStringList({
       tr("Genesis"),tr("Exodus"),tr("Leviticus"),tr("Numbers"),tr("Deuteronomy"),tr("Joshua"),tr("Judges"),tr("Ruth"),tr("1 Samuel"),tr("2 Samuel"),
       tr("1 Kings"),tr("2 Kings"),tr("1 Chroicles"),tr("2 Chronicles"),tr("Ezra"),tr("Nehemiah"),tr("Esther"),tr("Job"),tr("Psalms"),tr("Proverbs"),
       tr("Ecclesiastes"),tr("Song of Solomon"),tr("Isaiah"),tr("Jeremiah"),tr("Lamentations"),tr("Ezekiel"),tr("Daniel"),tr("Hosea"),tr("Joel"),tr("Amos"),
       tr("Obadiah"), tr("Jonah"), tr("Micah"), tr("Nahum"), tr("Habakkuk"), tr("Zephaniah"), tr("Haggai"), tr("Zechariah"), tr("Malachi"),
       tr("Matthew"),tr("Mark"),tr("Luke"),tr("John"),tr("Acts"),tr("Romans"),tr("1 Corinthians"),tr("2 Corinthians"),tr("Galatians"),
       tr("Ephesians"),tr("Philippians"),tr("Colossians"),tr("1 Thessalonians"),tr("2 Thessalonians"),tr("1 Timothy"),tr("2 Timothy"),tr("Titus"),tr("Philemon"),
       tr("Hebrews"),tr("James"),tr("1 Peter"),tr("2 Peter"),tr("1 John"),tr("2 John"),tr("3 John"),tr("Jude"),tr("Revelation")});

    settings = new QSettings("Mate-Solutions", "Bible-Mate");
    fontsize=settings->value("fontsize",16).toUInt();
    version=settings->value("version","nuovadiodati").toString();
    lastribbon=settings->value("lastribbon","Gen.1:1").toString();

    path_bibles = qApp->applicationDirPath()+"/bibles";
    path_notes = settings->value("path_notes",
                                 QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
    dbhelper = new DBHelper(path_bibles, path_notes,version);

    ui->book_CBox->addItems(books_name);

    setComboBox(lastribbon);

    QActionGroup* group = new QActionGroup( this );
    group->setExclusive(true);
    ui->action14->setActionGroup(group);
    ui->action16->setActionGroup(group);
    ui->action18->setActionGroup(group);
    ui->action20->setActionGroup(group);
    ui->action22->setActionGroup(group);

    QActionGroup* groupB = new QActionGroup( this );
    groupB->setExclusive(true);
    ui->actionNuova_Diodati->setActionGroup(groupB);
    ui->actionNew_International->setActionGroup(groupB);
    ui->actionCEI_2008->setActionGroup(groupB);
    ui->actionDiodati->setActionGroup(groupB);
    ui->actionKing_James->setActionGroup(groupB);
    ui->actionNew_King_James->setActionGroup(groupB);
    ui->actionNuova_Riveduta->setActionGroup(groupB);
    ui->actionSynodal->setActionGroup(groupB);

    switch (fontsize){
        case 14: ui->action14->setChecked(true);
        break;
        case 16: ui->action16->setChecked(true);
        break;
        case 18: ui->action18->setChecked(true);
        break;
        case 20: ui->action20->setChecked(true);
        break;
        case 22: ui->action22->setChecked(true);
        break;
    }

    if (version=="nuovadiodati") ui->actionNuova_Diodati->setChecked(true);
    else if (version=="newinternationalversionus") ui->actionNew_International->setChecked(true);
    else if (version=="nuovariveduta") ui->actionNuova_Riveduta->setChecked(true);
    else if (version=="cei2008") ui->actionCEI_2008->setChecked(true);
    else if (version=="diodati") ui->actionDiodati->setChecked(true);
    else if (version=="kingjamesversion") ui->actionKing_James->setChecked(true);
    else if (version=="newkingjamesversion") ui->actionNew_King_James->setChecked(true);
    else if (version=="synodal") ui->actionSynodal->setChecked(true);

    //ui->pushButton->click();
    ui->pushButton_7->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setComboBox(QString ref)
{
    int idx = books_id.indexOf(ref.split(".")[0]);
    ui->book_CBox->setCurrentIndex(idx);

    int book_chaps = dbhelper->getNumChapters(ref.split(".")[0]);
    for (int i=1; i<=book_chaps; i++){
        chapters.push_back(QString::number(i));
    }
    ui->chapter_CBox->addItems(chapters);
    ui->chapter_CBox->setCurrentText(ref.split(".")[1].split(":")[0]);

    int num_verses = dbhelper->getNumVerses(
                ref.split(".")[0],
                ref.split(".")[1].split(":")[0]);
    for (int i=1; i<=num_verses; i++){
        verses.push_back(QString::number(i));
    }
    ui->verse_CBox->addItems(verses);
    ui->verse_CBox->setCurrentText(ref.split(".")[1].split(":")[1]);

}


/**
 * Change in selection of verses qlistview will produce a change in
 * single note textview in TAB1
 *
 * @brief MainWindow::Update_tab1
 * @param current
 * @param previous
 */
void MainWindow::Update_tab1(const QModelIndex &current, const QModelIndex &previous)       //SELECT A VERSE IN TAB1
{
    if (!ui->checkBox_2->isChecked()){
        //if (ui->plainTextEdit->toPlainText()!=""||ui->lineEdit_3->text()!=""){
        if (previous.row()!=-1 && selNote_T1->editing){
            //save note temporary
            selNote_T1->title=ui->lineEdit_3->text();
            selNote_T1->text=ui->plainTextEdit->toPlainText();

            bool note_exist=false;
            for (int i=0; i<(int)allNotes_T1.size(); i++){
                if (allNotes_T1[i].getRef() == selNote_T1->getRef()){
                    allNotes_T1[i].title = selNote_T1->title;
                    allNotes_T1[i].text = selNote_T1->text;
                    allNotes_T1[i].editing = selNote_T1->editing;
                    note_exist=true;
                    break;
                }
            }
            if (!note_exist) allNotes_T1.push_back(*selNote_T1);

            //ui->plainTextEdit->clear();
            //ui->lineEdit_3->clear();
        }

        ui->plainTextEdit->clear();
        ui->lineEdit_3->clear();

        qDebug() << "changed" << "current" << current.row() << "(" << index.at(current.row()) << ")" << "previous" << previous.row();

        selNote_T1 = new Note();
        selNote_T1->id = 0;
        selNote_T1->book_id = books_id.at(ui->book_CBox->currentIndex());
        selNote_T1->chap = index.at(current.row()).split(":")[0].toInt();
        selNote_T1->verse = index.at(current.row()).split(":")[1].toInt();
        selNote_T1->editing = false;
        for (Note note:allNotes_T1){
            if (note.chap==selNote_T1->chap && note.verse==selNote_T1->verse){
                //selNote = &note;
                selNote_T1->id =note.id;
                selNote_T1->text = note.text;
                selNote_T1->title = note.title;
                selNote_T1->note_date = note.note_date;
                selNote_T1->ribbon_date = note.ribbon_date;
                selNote_T1->ribbon_title = note.ribbon_title;
                selNote_T1->ribbon = note.ribbon;
                ui->lineEdit_3->setText(selNote_T1->title);
                ui->plainTextEdit->appendPlainText(selNote_T1->text);
                selNote_T1->editing = note.editing;
                break;
            }
        }
        if (selNote_T1->editing){
            ui->label_16->setText(tr("not saved"));
            ui->pushButton_3->setEnabled(true);
        }
        else {
            ui->label_16->setText("");
            ui->pushButton_3->setEnabled(false);
        }
    }

    ui->pushButton_7->setEnabled(true);
    ui->pushButton_13->setEnabled(true);
}

void MainWindow::Update_tab4(const QModelIndex &current, const QModelIndex &previous)   //SELECT A RIBBON IN TAB4
{
    qDebug() << "changed" << "current" << current.row() << "previous" << previous.row();
}

/**
 * Change in selection of note qlistview will produce a change
 * in single note textview in TAB3
 *
 * @brief MainWindow::Update_tab3
 * @param current
 * @param previous
 */
void MainWindow::Update_tab3(const QModelIndex &current, const QModelIndex &previous)   //SELECT A NOTE IN TAB3
{
    qDebug() << "changed" << "current" << current.row() << "previous" << previous.row();

    selNote_T3 = &allNotes_T3[current.row()];

    ui->lineEdit_2->setText(selNote_T3->title);
    ui->plainTextEdit_2->clear();
    ui->plainTextEdit_2->appendPlainText(selNote_T3->text);
    ui->label_12->setText(selNote_T3->note_date);
    ui->label_14->setText(selNote_T3->getRef());
}

void MainWindow::Update_tab2(const QModelIndex &current, const QModelIndex &previous)   //SELECT A VERSE IN TAB2
{
    qDebug() << "changed" << "current" << results[current.row()].getRef() << "previous" << previous.row();

}

/**
 * Button to get the book and update the verses qlistview in TAB1
 *
 * @brief MainWindow::on_pushButton_clicked
 */
void MainWindow::on_pushButton_clicked()                                                //UPDATE READING TAB1
{
    index.clear();

    QString book_sel = books_id.at(ui->book_CBox->currentIndex());
    QString book_name;
    for (int i=0;i<books_id.size();i++){
        if (books_id[i]==book_sel){
            book_name = books_name[i];
            break;
        }
    }
    book = dbhelper->GetBook(version, book_sel);

    /*int book_chaps = dbhelper->getNumChapters(book_sel);
    chapters.clear();
    for (int i=1; i<=book_chaps; i++){
        chapters.push_back(QString::number(i));
    }
    ui->chapter_CBox->clear();
    ui->chapter_CBox->addItems(chapters);
    */

    allNotes_T1 = dbhelper->getAllNotes(true);

    QStandardItemModel *model_tab1 = new QStandardItemModel();

    ListViewDelegate *listdelegate_tab1;    
    listdelegate_tab1 = new ListViewDelegate(fontsize, fontsize+3);

    ui->listView->setItemDelegate(listdelegate_tab1);
    ui->listView->setModel(model_tab1);
    //ui->listView->setSelectionMode(QAbstractItemView::MultiSelection);

    selectionModel = ui->listView->selectionModel();
    connect(selectionModel, &QItemSelectionModel::currentChanged, this, &MainWindow::Update_tab1);

    int cap=0;
    for (Verse newVerse:book.verses){
        bool noteMark=false;
        bool ribbonMark=false;
        for (Note note:allNotes_T1){
            if (note.getRef()==newVerse.getRef()){
                if (note.ribbon){
                    ribbonMark=true;
                }
                if (note.text!=""||note.title!=""){
                    noteMark=true;
                }
                break;
            }
        }
        QStandardItem *item = new QStandardItem();
        if (newVerse.chap!=cap) {

            QString temp;
            if (newVerse.chap==1){
                temp += book_name+"\n";
            }
            temp += QString(tr("\nChap.%1")).arg(newVerse.chap);
            if (newVerse.title!="")
                temp += " - " + newVerse.title;
            item->setData(temp,ListViewDelegate::HeaderRole);

            temp = QString("%1 ").arg(newVerse.verse_num);
            if (noteMark){
                //temp += QChar(0x2709) + QString(" ");
                temp += QChar(0x270E) + QString(" ");
            }
            if (ribbonMark){
                temp += QChar(0x2691) + QString(" ");
            }
            temp += newVerse.text;
            temp = temp.replace("<em>","").replace("</em>","");
            item->setData(temp,ListViewDelegate::SubheaderRole);

            cap=newVerse.chap;
        }
        else {
            QString temp = "\n"+newVerse.title;
            item->setData(temp,ListViewDelegate::HeaderRole);

            temp = QString("%1 ").arg(newVerse.verse_num);
            if (noteMark){
                temp += QChar(0x270E) + QString(" ");
            }
            if (ribbonMark){
                temp += QChar(0x2691) + QString(" ");
            }
            temp += newVerse.text;
            temp=temp.replace("<em>","").replace("</em>","");            
            //QPixmap bgPixmap(":/res/note1.png");
            //QPixmap scaled = bgPixmap.scaled( QSize(10, 10), Qt::KeepAspectRatio, Qt::SmoothTransformation );
            //QIcon icon(scaled);
            //item->setIcon(icon);
            item->setData(temp,ListViewDelegate::SubheaderRole);

        }
        index.push_back(QString("%1:%2").arg(newVerse.chap).arg(newVerse.verse_num));
        //index.push_back(QString::fromStdString(std::to_string(newVerse.chap)+":"+std::to_string(newVerse.verse_num)));
        model_tab1->appendRow(item);
    }

    if (!ui->checkBox_2->isChecked()){
        ui->plainTextEdit->clear();
        ui->lineEdit_3->clear();
    }

    QString ref = ui->chapter_CBox->currentText()+":"+ui->verse_CBox->currentText();
    for (int i=0;i<(int)index.size();i++){
        if (index[i]==ref){
            QModelIndex qmIndex = model_tab1->index(i,0);
            ui->listView->scrollTo(qmIndex, QAbstractItemView::PositionAtTop);
            break;
        }
    }

    ui->pushButton_7->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
}

/**
 * Button to save the changes in single note textview in TAB1
 *
 * @brief MainWindow::on_pushButton_3_clicked
 */
void MainWindow::on_pushButton_3_clicked()                                                  //SAVE NOTE IN TAB1
{
    /*
    Note note;
    QString chap = index.at(ui->listView->currentIndex().row()).split(":")[0];
    QString verse = index.at(ui->listView->currentIndex().row()).split(":")[1];
    QString book_id = ui->book_CBox->currentText();
    note = dbhelper->findNote(book_id+"."+chap+":"+verse);    
    */


    selNote_T1->note_date=today;
    selNote_T1->text=ui->plainTextEdit->toPlainText();
    selNote_T1->text.remove("**UNSAVED**");
    selNote_T1->title=ui->lineEdit_3->text();
    selNote_T1->editing=false;
    if (selNote_T1->id==0){
        selNote_T1->id = dbhelper->insertNote(*selNote_T1);
    }
    else{
        dbhelper->updateNote(*selNote_T1);
    }

    bool note_exist=false;
    for (int i=0;i<(int)allNotes_T1.size();i++){
        if (allNotes_T1[i].getRef()==selNote_T1->getRef()){
            allNotes_T1[i].id = selNote_T1->id;
            allNotes_T1[i].text = selNote_T1->text;
            allNotes_T1[i].title = selNote_T1->title;
            allNotes_T1[i].note_date = selNote_T1->note_date;
            allNotes_T1[i].editing = selNote_T1->editing;
            ui->label_16->setText("");
            ui->pushButton_3->setEnabled(false);
            note_exist=true;
            break;
        }
    }
    if (!note_exist) {
        allNotes_T1.push_back(*selNote_T1);
    }

    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText(selNote_T1->text);
    ui->lineEdit_3->setText(selNote_T1->title);

    //ui->pushButton->click();
}

/**
 * Button to serch the text in the whole Bible in TAB2
 *
 * @brief MainWindow::on_pushButton_2_clicked
 */
void MainWindow::on_pushButton_2_clicked()                                                      //SEARCH TEXT IN TAB2
{
    QString temp=ui->lineEdit->text();
    QStringList templist = temp.split(" ");
    results=dbhelper->searchText(templist, ui->checkBox->isChecked());

    ui->label_8->setText(QString("%1").arg(results.size()));

    QStandardItemModel *model_tab2 = new QStandardItemModel();

    ListViewDelegate_2 *listdelegate_tab2;
    listdelegate_tab2 = new ListViewDelegate_2(16,19);

    ui->listView_2->setItemDelegate(listdelegate_tab2);
    ui->listView_2->setModel(model_tab2);

    for (Verse verse:results){
        QStandardItem *item = new QStandardItem();

        item->setData("\n",ListViewDelegate_2::HeaderRole);
        QString temp = QString(verse.text+" ("+verse.book+".%1:%2)\n").arg(verse.chap).arg(verse.verse_num);
        temp=temp.replace("<em>","").replace("</em>","");

        item->setData(temp,ListViewDelegate_2::SubheaderRole);
        model_tab2->appendRow(item);
    }

    selectionModel = ui->listView_2->selectionModel();
    connect(selectionModel, &QItemSelectionModel::currentChanged, this, &MainWindow::Update_tab2);
}

/**
 * When enter is pressed in the lineEdit the button serach is clicked in TAB2
 *
 * @brief MainWindow::on_lineEdit_returnPressed
 */
void MainWindow::on_lineEdit_returnPressed()
{
    ui->pushButton_2->click();
}

int getIndexOfBook(QString book)
{
    for (int i=0;i<books_id.size();i++){
        if (books_id.at(i)==book){
            return i+1;
        }
    }
    return -1;
}

bool compareFunction_date (Note a, Note b) {return a.note_date<b.note_date;}
bool compareFunction_title (Note a, Note b) {return a.title<b.title;}
bool compareFunction_ribbontitle (Note a, Note b) {return a.ribbon_title<b.ribbon_title;}
bool compareFunction_ref (Note a, Note b)
{
    QString tempA=a.getRef();
    QString tempB=b.getRef();
    QString bookA=tempA.split(".")[0];
    QString bookB=tempB.split(".")[0];
    int valA=getIndexOfBook(bookA)*1000000+
            tempA.split(".")[1].split(":")[0].toUInt()*1000+
            tempA.split(".")[1].split(":")[1].toUInt();
    int valB=getIndexOfBook(bookB)*1000000+
            tempB.split(".")[1].split(":")[0].toUInt()*1000+
            tempB.split(".")[1].split(":")[1].toUInt();

    return valA<valB;
}


/**
 * Button to update the qlistview of notes in TAB3
 *
 * @brief MainWindow::on_pushButton_5_clicked
 */
void MainWindow::on_pushButton_5_clicked()                                                          //UPDATE IN TAB3
{
    //Apply FILTERS to allNotes
    if (ui->lineEdit_4->text()!=""){
        QStringList filter=ui->lineEdit_4->text().split(" ");
        allNotes_T3 = dbhelper->getAllNotes(filter,false);
    }
    else{
        allNotes_T3 = dbhelper->getAllNotes(false);
    }
    if (ui->radioButton->isChecked())
        std::sort(allNotes_T3.begin(), allNotes_T3.end(), compareFunction_date);
    else if (ui->radioButton_2->isChecked())
        std::sort(allNotes_T3.begin(), allNotes_T3.end(), compareFunction_title);
    else
        std::sort(allNotes_T3.begin(), allNotes_T3.end(), compareFunction_ref);

    //LOAD NOTES MANAGER LIST
    QStandardItemModel *model_tab3 = new QStandardItemModel();

    ListViewDelegate_2 *listdelegate_tab3;
    listdelegate_tab3 = new ListViewDelegate_2(14,16);

    ui->listView_3->setItemDelegate(listdelegate_tab3);
    ui->listView_3->setModel(model_tab3);

    selectionModel = ui->listView_3->selectionModel();
    connect(selectionModel, &QItemSelectionModel::currentChanged, this, &MainWindow::Update_tab3);

    for (Note note:allNotes_T3){
        QStandardItem *item = new QStandardItem();
        item->setData(note.title,ListViewDelegate_2::HeaderRole);
        QString subtext(note.text);
        subtext=subtext.replace("\n"," ").mid(0,35)+"...";
        item->setData(subtext,ListViewDelegate_2::SubheaderRole);
        model_tab3->appendRow(item);
    }

    ui->lineEdit_2->clear();
    ui->plainTextEdit_2->clear();
    ui->label_12->setText("[date]");
    ui->label_14->setText("[ref]");
}




void MainWindow::on_actionNuova_Diodati_triggered()
{
    version="nuovadiodati";
    settings->setValue("version","nuovadiodati");
    settings->sync();
    dbhelper = new DBHelper(path_bibles,path_notes,version);
    ui->pushButton->click();
}


void MainWindow::on_actionNew_International_triggered()
{
    version="newinternationalversionus";
    settings->setValue("version","newinternationalversionus");
    settings->sync();
    dbhelper = new DBHelper(path_bibles,path_notes,version);
    ui->pushButton->click();
}


void MainWindow::on_action22_triggered()
{
    fontsize=22;
    settings->setValue("fontsize",fontsize);
    settings->sync();
    ui->pushButton->click();
}


void MainWindow::on_action20_triggered()
{
    fontsize=20;
    settings->setValue("fontsize",fontsize);
    settings->sync();
    ui->pushButton->click();
}


void MainWindow::on_action18_triggered()
{
    fontsize=18;
    settings->setValue("fontsize",fontsize);
    settings->sync();
    ui->pushButton->click();
}


void MainWindow::on_action16_triggered()
{
    fontsize=16;
    settings->setValue("fontsize",fontsize);
    settings->sync();
    ui->pushButton->click();
}


void MainWindow::on_action14_triggered()
{
    fontsize=14;
    settings->setValue("fontsize",fontsize);
    settings->sync();
    ui->pushButton->click();
}


void MainWindow::on_actionAbout_triggered()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("About"));
    msgBox.setText(tr("Bible Mate v.0.1\nFree to read and study the Bible."));
    msgBox.setIconPixmap(QPixmap(":/res/img/ic_launcher.png"));
    msgBox.exec();

}


void MainWindow::on_actionKing_James_triggered()
{
    version="kingjamesversion";
    settings->setValue("version","kingjamesversion");
    settings->sync();
    dbhelper = new DBHelper(path_bibles,path_notes,version);
    ui->pushButton->click();
}


void MainWindow::on_actionNew_King_James_triggered()
{
    version="newkingjamesversion";
    settings->setValue("version","newkingjamesversion");
    settings->sync();
    dbhelper = new DBHelper(path_bibles,path_notes,version);
    ui->pushButton->click();
}


void MainWindow::on_actionCEI_2008_triggered()
{
    version="cei2008";
    settings->setValue("version","cei2008");
    settings->sync();
    dbhelper = new DBHelper(path_bibles,path_notes,version);
    ui->pushButton->click();
}


void MainWindow::on_actionNuova_Riveduta_triggered()
{
    version="nuovariveduta";
    settings->setValue("version","nuovariveduta");
    settings->sync();
    dbhelper = new DBHelper(path_bibles,path_notes,version);
    ui->pushButton->click();
}


void MainWindow::on_actionDiodati_triggered()
{
    version="diodati";
    settings->setValue("version","diodati");
    settings->sync();
    dbhelper = new DBHelper(path_bibles,path_notes,version);
    ui->pushButton->click();
}


void MainWindow::on_actionSynodal_triggered()
{
    version="synodal";
    settings->setValue("version","synodal");
    settings->sync();
    dbhelper = new DBHelper(path_bibles,path_notes,version);
    ui->pushButton->click();
}


void MainWindow::on_actionExit_triggered()
{
    QString ref;
    ref+=books_id.at(ui->book_CBox->currentIndex())+".";
    ref+=index.at(ui->listView->indexAt(QPoint(0,0)).row());
    qDebug()<<ref;
    settings->setValue("lastribbon",ref);

    this->close();
}

/**
 * Save the note in the notes management TAB3
 *
 * @brief MainWindow::on_pushButton_4_clicked
 */
void MainWindow::on_pushButton_4_clicked()
{    
    selNote_T3->note_date=today;
    selNote_T3->text=ui->plainTextEdit_2->toPlainText();
    selNote_T3->title=ui->lineEdit_2->text();

    dbhelper->updateNote(*selNote_T3);

}


void MainWindow::on_pushButton_6_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  tr("Delete current note"),
                                  tr("Are you sure you want to delete the current note?"),
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        qDebug() << "YES was clicked";
        dbhelper->deleteNote(*selNote_T3);

        selNote_T3 = new Note();

        ui->plainTextEdit_2->clear();
        ui->lineEdit_2->clear();
        ui->label_12->setText("[Date]");
        ui->label_14->setText("[Ref]");

        ui->pushButton_5->click();

      } else {
        qDebug() << "NO was clicked";

      }
}


void MainWindow::on_chapter_CBox_currentTextChanged(const QString &arg1)
{
    static int count=0;
    if (count>0){
        qDebug()<<arg1;
        QString book_sel = books_id.at(ui->book_CBox->currentIndex());
        int num_verses = dbhelper->getNumVerses(book_sel, arg1);
        verses.clear();
        for (int i=1; i<=num_verses; i++){
            verses.push_back(QString::number(i));
        }
        ui->verse_CBox->clear();
        ui->verse_CBox->addItems(verses);
    }
    count++;
}


void MainWindow::on_pushButton_7_clicked()                                          //DELETE RIBBON SELECTED
{
    if (selNote_T1->ribbon){
        selNote_T1->ribbon=false;
        selNote_T1->ribbon_date="";
        selNote_T1->ribbon_title="";
        dbhelper->updateNote(*selNote_T1);

        QString book_id = books_id.at(ui->book_CBox->currentIndex());
        QString ref=book_id+"."+index.at(ui->listView->indexAt(QPoint(0,0)).row());
        setComboBox(ref);
        ui->pushButton->click();
    }
}


void MainWindow::on_book_CBox_currentIndexChanged(int index)
{
    static int count=0;
    if (count>0){
        qDebug()<<books_id.at(index);
        int book_chaps = dbhelper->getNumChapters(books_id.at(index));
        chapters.clear();
        for (int i=1; i<=book_chaps; i++){
            chapters.push_back(QString::number(i));
        }
        ui->chapter_CBox->clear();
        ui->chapter_CBox->addItems(chapters);
    }
    count++;
}



void MainWindow::on_pushButton_8_clicked()
{
    QString ref = results[ui->listView_2->currentIndex().row()].getRef();

    setComboBox(ref);
    ui->pushButton->click();

    ui->tabWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_9_clicked()
{
    QPrinter printer;
    QPrintPreviewDialog *dialog = new QPrintPreviewDialog(&printer);
    dialog->setWindowTitle(tr("Print note"));
    connect(dialog,SIGNAL(paintRequested(QPrinter*)),this,SLOT(print_T1(QPrinter*)));
    dialog->exec();
}

void MainWindow::print_T1(QPrinter *p)
{
    QTextDocument document;
    QString temp =ui->plainTextEdit->toPlainText();
    temp.replace("\n\n","</p><p>");
    temp.replace("\n","<br>");
    QString html=
            "<div align=right>"
                "Date:"+selNote_T1->note_date+"<br>"
            "</div>"
            "<div align=left>"
                "Ref.:"+selNote_T1->getRef()+"<br>"
            "</div>"
            "<H1 align=center>"+ui->lineEdit_3->text()+"</H1>"
            "<p align=justify>"+temp+"</p>";
    document.setHtml(html);
    document.print(p);
}

void MainWindow::print_T3(QPrinter *p)
{
    QTextDocument document;
    QString temp =ui->plainTextEdit_2->toPlainText();
    temp.replace("\n\n","</p><p>");
    temp.replace("\n","<br>");
    QString html=
            "<div align=right>"
                "Date:"+ui->label_12->text()+"<br>"
            "</div>"
            "<div align=left>"
                "Ref.:"+ui->label_14->text()+"<br>"
            "</div>"
            "<H1 align=center>"+ui->lineEdit_2->text()+"</H1>"
            "<p align=justify>"+temp+"</p>";
    document.setHtml(html);
    document.print(p);
}


void MainWindow::on_pushButton_10_clicked()
{
    QPrinter printer;
    QPrintPreviewDialog *dialog = new QPrintPreviewDialog(&printer);
    dialog->setWindowTitle(tr("Print note"));
    connect(dialog,SIGNAL(paintRequested(QPrinter*)),this,SLOT(print_T3(QPrinter*)));
    dialog->exec();
}




bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->plainTextEdit)
    {
        if(event->type() == QKeyEvent::KeyPress)
        {
            selNote_T1->editing=true;
            ui->label_16->setText(tr("not saved"));
            ui->pushButton_3->setEnabled(true);
            //QKeyEvent * ke = static_cast<QKeyEvent*>(event);
            //if(ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter)
            //{
            //    return true; // do not process this event further
            //}
        }
        return false; // process this event further
    }
    else
    {
        // pass the event on to the parent class
        return QMainWindow::eventFilter(watched, event);
    }
}


void MainWindow::on_actionNotes_file_location_triggered()
{
    path_notes = QFileDialog::getExistingDirectory (this, tr("Directory"), path_notes);
    if ( path_notes.isNull() == false )
    {
        settings->setValue("path_notes",path_notes);
        dbhelper = new DBHelper(path_bibles,path_notes,version);
        ui->pushButton->click();
    }
}


void MainWindow::on_pushButton_13_clicked()
{
    if (!selNote_T1->ribbon){
        bool ok;
        QString text = QInputDialog::getText(0, tr("New ribbon"),
                                                tr("Ribbon title:"), QLineEdit::Normal,
                                                "", &ok);
        if (ok){
            selNote_T1->ribbon_title=text;
            selNote_T1->ribbon_date=today;
            selNote_T1->ribbon=true;
            if (selNote_T1->id==0)
                selNote_T1->id=dbhelper->insertNote(*selNote_T1);
            else
                dbhelper->updateNote(*selNote_T1);

            QString book_id = books_id.at(ui->book_CBox->currentIndex());
            QString ref=book_id+"."+index.at(ui->listView->indexAt(QPoint(0,0)).row());
            setComboBox(ref);
            ui->pushButton->click();
        }
    }
    else{
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("New Ribbon"));
        msgBox.setText(tr("The ribbon is already here."));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }
}


void MainWindow::on_pushButton_11_clicked()                                                     //UPDATE IN TAB4
{
    allNotes_T4 = dbhelper->getAllRibbons();
    std::sort(allNotes_T4.begin(), allNotes_T4.end(), compareFunction_ribbontitle);

    //LOAD NOTES MANAGER LIST
    QStandardItemModel *model_tab4 = new QStandardItemModel();

    ListViewDelegate_4 *listdelegate_tab4;
    listdelegate_tab4 = new ListViewDelegate_4(14,14);

    ui->listView_4->setItemDelegate(listdelegate_tab4);
    ui->listView_4->setModel(model_tab4);

    selectionModel = ui->listView_4->selectionModel();
    connect(selectionModel, &QItemSelectionModel::currentChanged, this, &MainWindow::Update_tab4);

    QString rtitle="";
    for (Note note:allNotes_T4){
        QStandardItem *item = new QStandardItem();

        if (note.ribbon_title!=rtitle){
            item->setData(note.ribbon_title,ListViewDelegate_4::HeaderRole);
            item->setData("   "+note.getRef(),ListViewDelegate_4::SubheaderRole);

            rtitle=note.ribbon_title;
        }
        else{
            item->setData("\n",ListViewDelegate_4::HeaderRole);
            item->setData("   "+note.getRef(),ListViewDelegate_4::SubheaderRole);
        }

        model_tab4->appendRow(item);
    }

}


void MainWindow::on_pushButton_12_clicked()
{
    QString ref = allNotes_T4[ui->listView_4->currentIndex().row()].getRef();

    setComboBox(ref);
    ui->pushButton->click();

    ui->tabWidget->setCurrentIndex(0);
}

