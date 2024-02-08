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
#include <QClipboard>
#include <QTimer>
#include <QProcess>
#include <QThread>
#include <QSound>
#include <QXmlStreamReader>
#include <algorithm>

QString app_release = "0.1.18";

QProcess * process1;
QSound * player;
Verse verse_of_the_day;

QString localpath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/BibleMate";
QString language;
QStringList chapters, verses;
QDateTime date = QDateTime::currentDateTime();
QString today = date.toString("yyyy.MM.dd");
QSettings settings("Mate-Solutions", "Bible-Mate");
DBHelper dbhelper;
Note selNote_T1, selNote_T3, selNote_T4;
Verse selVerse;
const QColor yellow	( 255, 255,   0 );

QStringList books_id = {
    "Gen","Exod","Lev","Num","Deut","Josh","Judg","Ruth","1Sam","2Sam",
    "1Kgs","2Kgs","1Chr","2Chr","Ezra","Neh","Esth","Job","Ps","Prov",
    "Eccl","Song","Isa","Jer","Lam","Ezek","Dan","Hos","Joel","Amos",
    "Obad","Jonah","Mic","Nah","Hab","Zeph","Hag","Zech","Mal",
    "Matt","Mark","Luke","John","Acts","Rom","1Cor","2Cor","Gal",
    "Eph","Phil","Col","1Thess","2Thess","1Tim","2Tim","Titus","Phlm",
    "Heb","Jas","1Pet","2Pet","1John","2John","3John","Jude","Rev"};

QStringList promises = {
    "1John.2:25", "Rom.9:8", "Gen.13:15", "Heb.10:36", "Ps.105:8", "Ps.89:34", "Gen.28:15",
    "2Sam.7:25", "Acts.2:39", "Rom.4:16", "2Cor.1:20", "Heb.6:12", "Heb.10:23", "2Pet.1:4",
    "Josh.21:45", "Ps.12:6", "Eph.2:12", "Ps.119:49", "Ps.119:148", "Gal.3:29", "Num.23:19",
    "Ps.18:30", "Eccl.5:5", "Heb.11:13", "2Pet.3:4", "Ps.85:8", "Eph.1:13", "Ps.119:103",
    "Ps.132:11", "Rom.4:21", "Eccl.5:4", "Rom.9:4", "Heb.8:6", "Heb.11:39", "Ps.119.50",
    "Heb.6:15", "2Pet.3:9", "2Cor.7:1", "Rom.4:13", "Deut.23:23", "Ps.106:12", "Gal.3:19",
    "Acts.7:17", "Heb.9:15", "Eph.6:2", "Jer.33:14", "Prov.25:14", "Luke.1:45", "2Cor.5:21",
    "Isa.53:5", "1John.1:9", "Acts.4:12", "Deut.8:3", "1Pet.1:13", "Ezek.36:26", "Ps.27:4"
};


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->plainTextEdit->installEventFilter(this);
    ui->plainTextEdit_2->installEventFilter(this);
    ui->lineEdit_5->installEventFilter(this);
    ui->listView->installEventFilter(this);

    // The thread and the worker are created in the constructor so it is always safe to delete them.
    thread = new QThread();
    worker = new Worker();

    worker->moveToThread(thread);
    //connect(worker, SIGNAL(valueChanged(QString)), ui->label, SLOT(setText(QString)));
    connect(worker, SIGNAL(workRequested()), thread, SLOT(start()));
    connect(thread, SIGNAL(started()), worker, SLOT(doWork()));
    //connect(worker, SIGNAL(finished()), thread, SLOT(quit()), Qt::DirectConnection);

    bible_versions = {
        {"it-IT", "Nuova Diodati", "nuovadiodati", "nuovadiodati.db", ":/res/img/it.png"},
        {"it-IT", "Nuova Riveduta", "nuovariveduta", "nuovariveduta.db", ":/res/img/it.png"},
        {"it-IT", "Diodati", "diodati","diodati.db",":/res/img/it.png"},
        {"it-IT", "Cei 2008", "cei2008","cei2008.db",":/res/img/it.png"},
        {"en-US", "English Standard", "esv","esv.db",":/res/img/us.png"},
        {"en-US", "New International", "newinternationalversionus","newinternationalversionus.db",":/res/img/us.png"},
        {"en-US", "New International 1984", "niv1984","niv1984.db",":/res/img/us.png"},
        {"en-US", "King James", "kingjamesversion","kingjamesversion.db",":/res/img/us.png"},
        {"en-US", "New King James", "newkingjamesversion","newkingjamesversion.db",":/res/img/us.png"},
        {"en-US", "American Standard", "americanstandardversion","americanstandardversion.db",":/res/img/us.png"},
        {"ru-RU", "Synodal", "synodal","synodal.db",":/res/img/ru.png"},
        {"es-ES", "Reina Valera", "reinavalera","reinavalera.db",":/res/img/es.png"},
        {"es-ES", "Sagradas Escrituras", "sagradasescrituras","sagradasescrituras.db",":/res/img/es.png"},
        {"es-ES", "Portuguese", "port","port.db",":/res/img/pt.png"},
        {"fr-FR", "La Bible Ostervald", "ostv1996","ostv1996.db",":/res/img/fr.png"},
        {"de-DE", "Elberfelder Bibel", "elberfelder","elberfelder.db",":/res/img/de.png"},
    };

    version = settings.value("version","nuovadiodati").toString();
    addImportedBibles();

    books_name = QStringList({
       tr("Genesis"),tr("Exodus"),tr("Leviticus"),tr("Numbers"),tr("Deuteronomy"),tr("Joshua"),tr("Judges"),tr("Ruth"),tr("1 Samuel"),tr("2 Samuel"),
       tr("1 Kings"),tr("2 Kings"),tr("1 Chroicles"),tr("2 Chronicles"),tr("Ezra"),tr("Nehemiah"),tr("Esther"),tr("Job"),tr("Psalms"),tr("Proverbs"),
       tr("Ecclesiastes"),tr("Song of Solomon"),tr("Isaiah"),tr("Jeremiah"),tr("Lamentations"),tr("Ezekiel"),tr("Daniel"),tr("Hosea"),tr("Joel"),tr("Amos"),
       tr("Obadiah"), tr("Jonah"), tr("Micah"), tr("Nahum"), tr("Habakkuk"), tr("Zephaniah"), tr("Haggai"), tr("Zechariah"), tr("Malachi"),
       tr("Matthew"),tr("Mark"),tr("Luke"),tr("John"),tr("Acts"),tr("Romans"),tr("1 Corinthians"),tr("2 Corinthians"),tr("Galatians"),
       tr("Ephesians"),tr("Philippians"),tr("Colossians"),tr("1 Thessalonians"),tr("2 Thessalonians"),tr("1 Timothy"),tr("2 Timothy"),tr("Titus"),tr("Philemon"),
       tr("Hebrews"),tr("James"),tr("1 Peter"),tr("2 Peter"),tr("1 John"),tr("2 John"),tr("3 John"),tr("Jude"),tr("Revelation")});

    fontsize = settings.value("fontsize",16).toUInt();
    lastribbon = settings.value("lastribbon","Gen.1:1").toString();

    ui->book_CBox->blockSignals(true);
    ui->chapter_CBox->blockSignals(true);
    //ui->version_CBox->blockSignals(true);

    ui->book_CBox->addItems(books_name);

    /*int id=0;
    for (Bible_version bl:bible_versions){
        ui->version_CBox->addItem(QIcon(bl.icon),bl.name);
        if (version==bl.version){
            ui->version_CBox->setCurrentIndex(id);
            language=bl.lang;
        }
        id++;
    }*/

    ui->book_CBox->blockSignals(false);
    ui->chapter_CBox->blockSignals(false);
    //ui->version_CBox->blockSignals(false);

    //path_bibles = qApp->applicationDirPath()+"/bibles";
    path_notes = settings.value("path_notes",
                                 QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
    dbhelper = DBHelper(path_notes,bible_versions.at(ui->version_CBox->currentIndex()));

    setComboBox(lastribbon);

    //qDebug() << "Step 1";
    QActionGroup* group = new QActionGroup( this );
    group->setExclusive(true);
    ui->action14->setActionGroup(group);
    ui->action16->setActionGroup(group);
    ui->action18->setActionGroup(group);
    ui->action20->setActionGroup(group);
    ui->action22->setActionGroup(group);

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

    //ui->pushButton->click();
    ui->pushButton_7->setEnabled(false);
    //qDebug() << "Step 4";

    srand(time(0));
    int range = 9 - 1 + 1;
    int num = rand() % range + 1;
    QString picture;
    if (num<10)
        picture=QString(":/res/pictures/img/pic0%1.jpg").arg(num);
    else
        picture=QString(":/res/pictures/img/pic%1.jpg").arg(num);
    QPixmap p = QPixmap( picture );
    int w = ui->label_20->width();
    int h = ui->label_20->height();
    ui->label_20->setPixmap( p.scaled(w,h,Qt::KeepAspectRatio) );

    range = promises.size()-1+1;
    num = rand() % range + 1;
    verse_of_the_day = dbhelper.GetVerse(version,promises.at(num-1));
    ui->label_21->setText(verse_of_the_day.text+" ("+verse_of_the_day.getRef()+")");
    ui->label_21->setWordWrap(true);

    QTimer::singleShot(200, this, SLOT(executeWhenVisible()));


}

void MainWindow::executeWhenVisible(){    
    ui->pushButton->click();
    ui->tabWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    /*if (process2!=NULL && process2->processId()>0){
        process2->terminate();
        process2->waitForFinished();
        delete process2;
        process2=NULL;
    }*/
    if (player!=NULL && !player->isFinished()){
        player->stop();
        delete player;
        player=NULL;
    }
    delete ui;
}

void MainWindow::addImportedBibles()
{
    QDir directory(localpath);
    QStringList files = directory.entryList(QStringList() << "*.db" << "*.DB",QDir::Files);
    for (QString file:files){
        if (file.left(1)=="_"){
            QString lang = file.split("_")[1];
            QString icon = ":/res/img/"+lang.split("-")[1].toLower()+".png";
            QString name = file.split("_")[2].remove(".db");
            QString filename = file;
            QString vers = file.split("_")[2].remove(".db");
            bool found=false;
            for (Bible_version bv:bible_versions){
                if (bv.name==name && bv.lang==lang){
                    found=true;
                    break;
                }
            }
            if (!found)
                bible_versions.push_back({lang,name,vers,filename,icon});
        }
    }
    ui->version_CBox->blockSignals(true);
    ui->version_CBox->clear();
    int id=0;
    for (Bible_version bl:bible_versions){
        ui->version_CBox->addItem(QIcon(bl.icon),bl.name);
        if (version==bl.version){
            ui->version_CBox->setCurrentIndex(id);
            language=bl.lang;
        }
        id++;
    }
    ui->version_CBox->blockSignals(false);
}

void MainWindow::setComboBox(QString ref)
{
    int idx = books_id.indexOf(ref.split(".")[0]);
    ui->book_CBox->setCurrentIndex(idx);

    int book_chaps = dbhelper.getNumChapters(ref.split(".")[0]);
    //int book_chaps = book.chap_vers.size();
    chapters.clear();
    for (int i=1; i<=book_chaps; i++){
        chapters.push_back(QString::number(i));
    }
    ui->chapter_CBox->clear();
    ui->chapter_CBox->addItems(chapters);
    ui->chapter_CBox->setCurrentText(ref.split(".")[1].split(":")[0]);

    int num_verses = dbhelper.getNumVerses(
                ref.split(".")[0],
                ref.split(".")[1].split(":")[0]);
    //int chap = ref.split(".")[1].split(":")[0].toInt();
    //int num_verses = book.chap_vers.at(chap-1);
    verses.clear();
    for (int i=1; i<=num_verses; i++){
        verses.push_back(QString::number(i));
    }
    ui->verse_CBox->clear();
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
    selVerse.text = book.verses.at(current.row()).text;
    selVerse.book = book.verses.at(current.row()).book;
    selVerse.chap = book.verses.at(current.row()).chap;
    selVerse.verse_num = book.verses.at(current.row()).verse_num;

    if (!ui->checkBox_2->isChecked()){

        //if (ui->plainTextEdit->toPlainText()!=""||ui->lineEdit_3->text()!=""){

        if (previous.row()!=-1 && selNote_T1.editing){
            //save note temporary
            selNote_T1.title = ui->lineEdit_3->text();
            //selNote_T1.text = ui->plainTextEdit->toPlainText();
            selNote_T1.text = ui->plainTextEdit->document()->toHtml();

            bool note_exist=false;
            for (int i=0; i<(int)allNotes_T1.size(); i++){
                if (allNotes_T1[i].getRef() == selNote_T1.getRef()){
                    allNotes_T1[i].title = selNote_T1.title;
                    allNotes_T1[i].text = selNote_T1.text;
                    allNotes_T1[i].editing = selNote_T1.editing;
                    note_exist=true;
                    break;
                }
            }
            if (!note_exist) allNotes_T1.push_back(selNote_T1);

            //ui->plainTextEdit->clear();
            //ui->lineEdit_3->clear();
        }

        ui->plainTextEdit->clear();
        ui->lineEdit_3->clear();

        qDebug() << "changed" << "current" << current.row() << "(" << index.at(current.row()) << ")" << "previous" << previous.row();

        ui->lineEdit_5->setText(book.verses.at(current.row()).link);

        selNote_T1 = Note();
        selNote_T1.id = 0;
        selNote_T1.book_id = books_id.at(ui->book_CBox->currentIndex());
        selNote_T1.chap = index.at(current.row()).split(":")[0].toInt();
        selNote_T1.verse = index.at(current.row()).split(":")[1].toInt();
        selNote_T1.editing = false;

        for (Note note:allNotes_T1){
            if (note.book_id==selNote_T1.book_id &&
                note.chap==selNote_T1.chap &&
                note.verse==selNote_T1.verse){

                selNote_T1.id =note.id;
                selNote_T1.text = note.text;
                selNote_T1.title = note.title;
                selNote_T1.note_date = note.note_date;
                selNote_T1.ribbon_date = note.ribbon_date;
                selNote_T1.ribbon_title = note.ribbon_title;
                selNote_T1.ribbon = note.ribbon;
                ui->lineEdit_3->setText(selNote_T1.title);
                //ui->plainTextEdit->setPlainText(selNote_T1.text);
                ui->plainTextEdit->appendHtml(selNote_T1.text);
                selNote_T1.editing = note.editing;                

                break;
            }
        }

        if (ui->plainTextEdit->toPlainText()!="" || ui->lineEdit_3->text()!="")
            ui->pushButton_9->setEnabled(true);
        else
            ui->pushButton_9->setEnabled(false);

        if (selNote_T1.editing){
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
    if (language!="ru-RU")
        ui->pushButton_24->setEnabled(true);
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

    selNote_T3 = allNotes_T3[current.row()];

    ui->lineEdit_2->setText(selNote_T3.title);
    ui->plainTextEdit_2->clear();
    ui->plainTextEdit_2->appendHtml(selNote_T3.text);
    ui->label_12->setText(selNote_T3.note_date);
    ui->label_14->setText(selNote_T3.getRef());

    ui->pushButton_6->setEnabled(true);
    ui->pushButton_10->setEnabled(true);
}

void MainWindow::Update_tab4(const QModelIndex &current, const QModelIndex &previous)   //SELECT A NOTE IN TAB4
{
    qDebug() << "changed" << "current" << current.row() << "previous" << previous.row();

    selNote_T4 = allNotes_T4[current.row()];

    ui->pushButton_12->setEnabled(true);
    ui->pushButton_14->setEnabled(true);
    ui->pushButton_15->setEnabled(true);

}

void MainWindow::Update_tab2(const QModelIndex &current, const QModelIndex &previous)   //SELECT A VERSE IN TAB2
{
    qDebug() << "changed" << "current" << results[current.row()].getRef() << "previous" << previous.row();

    ui->pushButton_8->setEnabled(true);

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
    if (book.book_id!=book_sel || book.version!=version){
        book = dbhelper.GetBook(version, book_sel);
    }

    /*int book_chaps = dbhelper.getNumChapters(book_sel);
    chapters.clear();
    for (int i=1; i<=book_chaps; i++){
        chapters.push_back(QString::number(i));
    }
    ui->chapter_CBox->clear();
    ui->chapter_CBox->addItems(chapters);
    */

    allNotes_T1 = dbhelper.getAllNotes(true);

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
    ui->pushButton_9->setEnabled(false);
    ui->pushButton_24->setEnabled(false);
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
    note = dbhelper.findNote(book_id+"."+chap+":"+verse);
    */


    selNote_T1.note_date = today;
    //selNote_T1.text = ui->plainTextEdit->toPlainText();
    selNote_T1.text = ui->plainTextEdit->document()->toHtml();
    selNote_T1.title = ui->lineEdit_3->text();
    selNote_T1.editing = false;
    if (selNote_T1.id == 0){
        selNote_T1.id = dbhelper.insertNote(selNote_T1);
    }
    else{
        dbhelper.updateNote(selNote_T1);
    }

    bool note_exist=false;
    for (int i=0;i<(int)allNotes_T1.size();i++){
        if (allNotes_T1[i].getRef() == selNote_T1.getRef()){
            allNotes_T1[i].id = selNote_T1.id;
            allNotes_T1[i].text = selNote_T1.text;
            allNotes_T1[i].title = selNote_T1.title;
            allNotes_T1[i].note_date = selNote_T1.note_date;
            allNotes_T1[i].editing = selNote_T1.editing;
            ui->label_16->setText("");
            ui->pushButton_3->setEnabled(false);
            note_exist=true;
            break;
        }
    }
    if (!note_exist) {
        allNotes_T1.push_back(selNote_T1);
    }
    QString book_id = books_id.at(ui->book_CBox->currentIndex());
    QString ref=book_id+"."+index.at(ui->listView->indexAt(QPoint(0,0)).row());
    setComboBox(ref);
    ui->pushButton->click();

    //ui->plainTextEdit->setPlainText(selNote_T1.text);
    //ui->lineEdit_3->setText(selNote_T1.title);

}

/**
 * Button to serch the text in the whole Bible in TAB2
 *
 * @brief MainWindow::on_pushButton_2_clicked
 */
void MainWindow::on_pushButton_2_clicked()                                                      //SEARCH TEXT IN TAB2
{
    QString temp=ui->lineEdit->text();

    if (temp!=""){

        QStringList templist = temp.split(" ");
        results=dbhelper.searchText(templist, ui->checkBox->isChecked());

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

        ui->pushButton_8->setEnabled(false);
    }

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
        allNotes_T3 = dbhelper.getAllNotes(filter,false);
    }
    else{
        allNotes_T3 = dbhelper.getAllNotes(false);
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
        QTextEdit temp;
        temp.setHtml(note.text);
        QString subtext(temp.toPlainText());
        subtext=subtext.replace("\n"," ").mid(0,35)+"...";
        item->setData(subtext,ListViewDelegate_2::SubheaderRole);
        model_tab3->appendRow(item);
    }

    ui->lineEdit_2->clear();
    ui->plainTextEdit_2->clear();
    ui->label_12->setText("[date]");
    ui->label_14->setText("[ref]");

    ui->pushButton_6->setEnabled(false);
    ui->pushButton_10->setEnabled(false);
}


void MainWindow::on_action22_triggered()
{
    fontsize=22;
    settings.setValue("fontsize",fontsize);

    ui->pushButton->click();
}


void MainWindow::on_action20_triggered()
{
    fontsize=20;
    settings.setValue("fontsize",fontsize);

    ui->pushButton->click();
}


void MainWindow::on_action18_triggered()
{
    fontsize=18;
    settings.setValue("fontsize",fontsize);

    ui->pushButton->click();
}


void MainWindow::on_action16_triggered()
{
    fontsize=16;
    settings.setValue("fontsize",fontsize);

    ui->pushButton->click();
}


void MainWindow::on_action14_triggered()
{
    fontsize=14;
    settings.setValue("fontsize",fontsize);

    ui->pushButton->click();
}


void MainWindow::on_actionAbout_triggered()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("About"));
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(tr("Bible Mate vers.")+ app_release + "<br><br>" + tr("Free offline program to read and study the Bible, the Holy Word of God.")+"<br><br>"+"<a href=\"https://www.paypal.com/donate/?hosted_button_id=DTG4LEFSQDDBL\">"+tr("Make a donation!")+"</a>");
    msgBox.setIconPixmap(QPixmap(":/res/img/ic_launcher.png"));
    msgBox.exec();

}


void MainWindow::on_actionExit_triggered()
{
    QString ref;
    ref+=books_id.at(ui->book_CBox->currentIndex())+".";
    ref+=index.at(ui->listView->indexAt(QPoint(0,0)).row());
    qDebug()<<ref;
    settings.setValue("lastribbon",ref);


    this->close();
}

/**
 * Save the note in the notes management TAB3
 *
 * @brief MainWindow::on_pushButton_4_clicked
 */
void MainWindow::on_pushButton_4_clicked()                                                  //SAVE NOTE in TAB3
{    
    selNote_T3.note_date=today;
    //selNote_T3.text=ui->plainTextEdit_2->toPlainText();
    selNote_T3.text = ui->plainTextEdit_2->document()->toHtml();
    selNote_T3.title=ui->lineEdit_2->text();
    selNote_T3.editing = false;

    dbhelper.updateNote(selNote_T3);
    ui->pushButton_4->setEnabled(false);
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
        dbhelper.deleteNote(selNote_T3);

        selNote_T3 = Note();

        ui->plainTextEdit_2->clear();
        ui->lineEdit_2->clear();
        ui->label_12->setText("[Date]");
        ui->label_14->setText("[Ref]");

        ui->pushButton_5->click();

      } else {
        qDebug() << "NO was clicked";

      }
}


void MainWindow::on_chapter_CBox_currentIndexChanged(int index)
{
    static int count=1;
    if (count>0){
        //qDebug()<<arg1;
        QString book_sel = books_id.at(ui->book_CBox->currentIndex());
        int num_verses = dbhelper.getNumVerses(book_sel, QString("%1").arg(index+1));
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
    if (selNote_T1.ribbon){
        selNote_T1.ribbon=false;
        selNote_T1.ribbon_date="";
        selNote_T1.ribbon_title="";
        dbhelper.updateNote(selNote_T1);

        QString book_id = books_id.at(ui->book_CBox->currentIndex());
        QString ref=book_id+"."+index.at(ui->listView->indexAt(QPoint(0,0)).row());
        setComboBox(ref);
        ui->pushButton->click();
    }
}


void MainWindow::on_book_CBox_currentIndexChanged(int index)
{
    static int count=1;
    if (count>0){
        qDebug()<<books_id.at(index);
        int book_chaps = dbhelper.getNumChapters(books_id.at(index));
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

    ui->tabWidget->setCurrentIndex(1);
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
    //QString temp =ui->plainTextEdit->();
    //temp.replace("\n\n","</p><p>");
    //temp.replace("\n","<br>");
    QString html=
            "<div align=right>"
                "Date:"+selNote_T1.note_date+"<br>"
            "</div>"
            "<div align=left>"
                "Ref.:"+selNote_T1.getRef()+"<br>"
            "</div>"
            "<H1 align=center>"+ui->lineEdit_3->text()+"</H1>"
            +selNote_T1.text;
    document.setHtml(html);
    document.print(p);
}

void MainWindow::print_T3(QPrinter *p)
{
    QTextDocument document;
    //QString temp =ui->plainTextEdit_2->toPlainText();
    //temp.replace("\n\n","</p><p>");
    //temp.replace("\n","<br>");
    QString html=
            "<div align=right>"
                "Date:"+selNote_T3.note_date+"<br>"
            "</div>"
            "<div align=left>"
                "Ref.:"+selNote_T3.getRef()+"<br>"
            "</div>"
            "<H1 align=center>"+selNote_T3.title+"</H1>"
            +selNote_T3.text;
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
    if (watched == ui->listView){
        if(event->type() == QKeyEvent::KeyPress)
        {
            QKeyEvent * ke = static_cast<QKeyEvent*>(event);
            if(ke->matches(QKeySequence::Copy))
            {
                QClipboard *clipboard = QApplication::clipboard();
                clipboard->setText(selVerse.text + " (" + selVerse.getRef()+")");
                return true; // do not process this event further
            }
            return false;
        }
        return false;
    }
    else if(watched == ui->plainTextEdit)
    {
        if(event->type() == QKeyEvent::KeyPress)
        {
            selNote_T1.editing=true;
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
    else if (watched == ui->plainTextEdit_2)
    {
        if(event->type() == QKeyEvent::KeyPress)
        {
            selNote_T3.editing=true;
            ui->pushButton_4->setEnabled(true);
        }
        return false;
    }
    else if (watched == ui->lineEdit_5){
        if(event->type() == QKeyEvent::KeyPress){
            QKeyEvent * ke = static_cast<QKeyEvent*>(event);
            if(ke->key() == Qt::Key_Right || ke->key() == Qt::Key_Left || ke->key() == Qt::Key_End || ke->key() == Qt::Key_Home)
            {
                return false; // contune to process this event
            }
            return true;    // do not process this event further
        }
        return false;
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
        settings.setValue("path_notes",path_notes);
        dbhelper = DBHelper(path_notes,bible_versions.at(ui->version_CBox->currentIndex()));
        ui->pushButton->click();
    }
}


void MainWindow::on_pushButton_13_clicked()
{
    if (!selNote_T1.ribbon){
        bool ok;
        QString text = QInputDialog::getText(0, tr("New ribbon"),
                                                tr("Ribbon title:"), QLineEdit::Normal,
                                                "", &ok);
        if (ok){
            selNote_T1.ribbon_title=text;
            selNote_T1.ribbon_date=today;
            selNote_T1.ribbon=true;
            if (selNote_T1.id==0)
                selNote_T1.id=dbhelper.insertNote(selNote_T1);
            else
                dbhelper.updateNote(selNote_T1);

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
    allNotes_T4 = dbhelper.getAllRibbons();
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

    ui->pushButton_12->setEnabled(false);
    ui->pushButton_14->setEnabled(false);
    ui->pushButton_15->setEnabled(false);

}

void MainWindow::on_pushButton_12_clicked()
{
    QString ref = allNotes_T4[ui->listView_4->currentIndex().row()].getRef();

    setComboBox(ref);
    ui->pushButton->click();

    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_16_clicked()
{
    QTextCursor cursor = ui->plainTextEdit->textCursor();
    if (cursor.hasSelection()){
        QTextCharFormat format;
        if (!cursor.charFormat().font().bold()){
            format.setFontWeight(QFont::Bold);

        }
        else{
            format.setFontWeight(QFont::Normal);
        }
        cursor.mergeCharFormat(format);

        selNote_T1.editing = true;
        ui->label_16->setText(tr("not saved"));
        ui->pushButton_3->setEnabled(true);
    }
}

void MainWindow::on_pushButton_17_clicked()
{
    QTextCursor cursor = ui->plainTextEdit->textCursor();
    if (cursor.hasSelection()){
        QTextCharFormat format;
        if (!cursor.charFormat().font().italic()){
            format.setFontItalic(true);

        }
        else{
            format.setFontItalic(false);
        }
        cursor.mergeCharFormat(format);

        selNote_T1.editing = true;
        ui->label_16->setText(tr("not saved"));
        ui->pushButton_3->setEnabled(true);
    }
}

void MainWindow::on_pushButton_18_clicked()
{
    QTextCursor cursor = ui->plainTextEdit->textCursor();
    if (cursor.hasSelection()){
        QTextCharFormat format;
        if (!cursor.charFormat().font().underline()){
            format.setFontUnderline(true);

        }
        else{
            format.setFontUnderline(false);
        }
        cursor.mergeCharFormat(format);

        selNote_T1.editing = true;
        ui->label_16->setText(tr("not saved"));
        ui->pushButton_3->setEnabled(true);
    }
}


void MainWindow::on_pushButton_19_clicked()
{
    QTextCursor cursor = ui->plainTextEdit_2->textCursor();
    if (cursor.hasSelection()){
        QTextCharFormat format;
        if (!cursor.charFormat().font().bold()){
            format.setFontWeight(QFont::Bold);

        }
        else{
            format.setFontWeight(QFont::Normal);
        }
        cursor.mergeCharFormat(format);

        selNote_T3.editing = true;
        ui->pushButton_4->setEnabled(true);
    }
}


void MainWindow::on_pushButton_20_clicked()
{
    QTextCursor cursor = ui->plainTextEdit_2->textCursor();
    if (cursor.hasSelection()){
        QTextCharFormat format;
        if (!cursor.charFormat().font().italic()){
            format.setFontItalic(true);

        }
        else{
            format.setFontItalic(false);
        }
        cursor.mergeCharFormat(format);

        selNote_T3.editing = true;
        ui->pushButton_4->setEnabled(true);
    }
}


void MainWindow::on_pushButton_21_clicked()
{
    QTextCursor cursor = ui->plainTextEdit_2->textCursor();
    if (cursor.hasSelection()){
        QTextCharFormat format;
        if (!cursor.charFormat().font().underline()){
            format.setFontUnderline(true);

        }
        else{
            format.setFontUnderline(false);
        }
        cursor.mergeCharFormat(format);

        selNote_T3.editing = true;
        ui->pushButton_4->setEnabled(true);
    }
}


void MainWindow::on_pushButton_22_clicked()
{
    QTextCursor cursor = ui->plainTextEdit->textCursor();
    if (cursor.hasSelection()){
        QTextCharFormat format;
        if (cursor.charFormat().background().color()!=yellow){
            format.setBackground(yellow);

        }
        else{
            format.setBackground(QColor(Qt::transparent));
        }
        cursor.mergeCharFormat(format);

        selNote_T1.editing = true;
        ui->label_16->setText(tr("not saved"));
        ui->pushButton_3->setEnabled(true);
    }
}


void MainWindow::on_pushButton_23_clicked()
{
    QTextCursor cursor = ui->plainTextEdit_2->textCursor();
    if (cursor.hasSelection()){
        QTextCharFormat format;
        if (cursor.charFormat().background().color()!=yellow){
            format.setBackground(yellow);

        }
        else{
            format.setBackground(QColor(Qt::transparent));
        }
        cursor.mergeCharFormat(format);

        selNote_T3.editing = true;
        ui->pushButton_4->setEnabled(true);
    }
}


void MainWindow::on_pushButton_15_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(0, tr("Edit ribbon"),
                                            tr("Ribbon title:"), QLineEdit::Normal,
                                            selNote_T4.ribbon_title, &ok);
    if (ok){
        selNote_T4.ribbon_title=text;
        selNote_T4.ribbon_date=today;
        dbhelper.updateNote(selNote_T4);

        ui->pushButton_11->click();
    }
}


void MainWindow::on_pushButton_24_clicked()             //Start reading
{
    if (player!=NULL && !player->isFinished()){
        player->stop();
        delete player;
        player=NULL;
        ui->pushButton_24->setIcon(QIcon(":/res/img/speech2.png"));
    }
    else{
        process1 = new QProcess(this);
        //process2 = new QProcess(this);
        QString texttoread;
        QString ref1 = selVerse.getRef();
        int chap = ref1.split(".")[1].split(":")[0].toInt();
        int last = book.chap_vers.at(chap-1);        
        QString ref2 = ref1.split(":")[0]+QString(":%1").arg(last);
        std::vector<Verse> temp = book.getPassage(ref1,ref2);
        for (Verse v:temp){
            texttoread+=v.text
                    .replace("DIO","Dio")
                    .replace("--",";")
                    .replace("-"," ")
                    .remove("\"")
                    .remove("»")
                    .remove("«")
                    .remove("<em>")
                    .remove("[")
                    .remove("]")
                    .remove("</em>")+" ";
        }        

        QString script1 = "pico2wave -l "+language+" -w="+localpath+"/test.wav \"" +
                texttoread + "\";";
        process1->start("/bin/sh", QStringList() << "-c" << script1);

        //qDebug()<<script1;
        qDebug()<<process1->error();
        process1->waitForFinished();
        delete process1;
        process1=NULL;

        player = new QSound(localpath+"/test.wav");
        player->play();
        ui->pushButton_24->setIcon(QIcon(":/res/img/stop.png"));

    }
}

void MainWindow::on_player_finished(){
    ui->pushButton_24->setIcon(QIcon(":/res/img/speech2.png"));
}


QLineEdit *ed2;
QComboBox *cbox1;
QWidget *w;

void MainWindow::on_actionImport_Osis_XML_Bible_triggered()
{
    QGridLayout *gridLayout = new QGridLayout;
    QLabel *lab0 = new QLabel(tr("You can import and use all Bibles in XML OSIS format.")+"\n"+tr("In the following link you can find a huge number of free Bibles."));
    QLabel *lab01 = new QLabel("<a href=\"https://github.com/bzerangue/osis-bibles/tree/master/\">osis-bibles</a>");
    lab01->setTextInteractionFlags(Qt::TextBrowserInteraction);
    lab01->setOpenExternalLinks(true);
    QLabel *lab1 = new QLabel(tr("Language")+":");
    cbox1 = new QComboBox();
    cbox1->addItems({"it-IT","en-US","en-GB","es-ES","fr-FR","de-DE"});
    cbox1->setMaximumWidth(80);
    QLabel *lab2 = new QLabel(tr("File:"));
    ed2 = new QLineEdit();
    QPushButton *select = new QPushButton("...");
    select->setMaximumWidth(30);
    QPushButton *import = new QPushButton(tr("Import"));
    QPushButton *cancel = new QPushButton(tr("Cancel"));
    import->setMaximumWidth(80);
    cancel->setMaximumWidth(80);
    pBar = new QProgressBar();
    pBar->setRange(0,65);
    pBar->setValue(0);

    gridLayout->addWidget(lab0,0,0,1,3);
    gridLayout->addWidget(lab01,1,0,1,3);
    gridLayout->addWidget(lab1,2,0);
    gridLayout->addWidget(cbox1,2,1);
    gridLayout->addWidget(lab2,3,0);
    gridLayout->addWidget(ed2,3,1);
    gridLayout->addWidget(select,3,2);
    gridLayout->addWidget(import,4,0);
    gridLayout->addWidget(cancel,4,1);
    gridLayout->addWidget(pBar,5,0,1,3);

    w = new QWidget();
    w->setLayout(gridLayout);
    w->setWindowTitle(tr("Import Bible - Osis XML file"));

    connect(worker, SIGNAL(valueChanged(int)), pBar, SLOT(setValue(int)));
    connect(worker, SIGNAL(finished()), this, SLOT(on_work_finished()));
    connect(select,SIGNAL(clicked()),this,SLOT(on_select_clicked()));
    connect(import,SIGNAL(clicked()),this,SLOT(on_import_clicked()));
    connect(cancel,SIGNAL(clicked()),this,SLOT(on_cancel_clicked()));

    w->show();

}

void MainWindow::on_work_finished()
{
    addImportedBibles();
    w->hide();
}

void MainWindow::on_select_clicked()
{
    QString path_osis = QFileDialog::getOpenFileName(this, tr("Imort Osis XML Bible"), localpath, tr("XML Files (*.xml)"));
    ed2->setText(path_osis);
}

void MainWindow::on_import_clicked()
{
    if (ed2->text()!=""){

        // To avoid having two threads running simultaneously, the previous thread is aborted.
        worker->abort();
        thread->wait(); // If the thread is not running, this will immediately return.
        worker->requestWork(ed2->text(),cbox1->currentText());
    }
}

void MainWindow::on_cancel_clicked()
{    
    w->hide();
}


void MainWindow::on_version_CBox_currentIndexChanged(int index)
{
    version=bible_versions.at(index).version;
    language=bible_versions.at(index).lang;
    settings.setValue("version",version);

    dbhelper = DBHelper(path_notes,bible_versions.at(ui->version_CBox->currentIndex()));
    ui->pushButton->click();
}


void MainWindow::on_pushButton_25_clicked()
{
    QString texttoread=verse_of_the_day.text
            .replace("DIO","Dio")
            .replace("--",";")
            .replace("-"," ")
            .remove("\"")
            .remove("»")
            .remove("«")
            .remove("<em>")
            .remove("[")
            .remove("]")
            .remove("</em>")+" ";
    QString script1 = "pico2wave -l "+language+" -w="+localpath+"/test_day.wav \"" +
            texttoread + "\";";
    QProcess * process2 = new QProcess();
    process2->start("/bin/sh", QStringList() << "-c" << script1);

    //qDebug()<<script1;
    qDebug()<<process2->error();
    process2->waitForFinished();
    delete process2;
    process2=NULL;

    if (player!=NULL && !player->isFinished()){
        player->stop();
        delete player;
        player=NULL;
    }
    player = new QSound(localpath+"/test_day.wav");
    player->play();
}

