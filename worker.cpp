#include "worker.h"
#include <QTimer>
#include <QEventLoop>

#include <QThread>
#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>
#include <QSqlDatabase>
#include "dbhelper.h"

Worker::Worker(QObject *parent) :
    QObject(parent)
{
    this->_working =false;
    this->_abort = false;

}

void Worker::requestWork(QString xmlfilepath, QString lang)
{
    this->xmlfilepath=xmlfilepath;
    this->lang=lang;

    this->mutex.lock();
    this->_working = true;
    this->_abort = false;
    qDebug()<<"Request worker start in Thread "<<thread()->currentThreadId();
    this->mutex.unlock();

    emit workRequested();
}

void Worker::abort()
{
    this->mutex.lock();
    if (this->_working) {
        this->_abort = true;
        qDebug()<<"Request worker aborting in Thread "<<thread()->currentThreadId();
    }
    this->mutex.unlock();
}

/*void Worker::doWork()
{
    qDebug()<<"Starting worker process in Thread "<<thread()->currentThreadId();

    //Parse the file
    QFile xmlFile(this->xmlfilepath);
    if (!xmlFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file " << this->xmlfilepath << " for reading";
    }
    QXmlStreamReader xmlReader(&xmlFile);

    QString type, ref, text, version;
    Book osis_book=Book();
    Verse verse;
    QString book_id = "";
    QSqlDatabase new_db;

    int book_count=0;

    int c_num,v_num;

    while(!xmlReader.atEnd()) {
        // Checks if the process should be aborted
        this->mutex.lock();
        bool abort = this->_abort;
        this->mutex.unlock();
        if (abort) {
            qDebug()<<"Aborting worker process in Thread "<<thread()->currentThreadId();
            break;
        }

        xmlReader.readNext();

        if (xmlReader.isStartDocument()) {
            continue;
        }

        if (xmlReader.isEndDocument()) {
            break;
        }

        if (xmlReader.isStartElement()){
            QStringRef elementName = xmlReader.name();
            if (elementName == "work"){
                QXmlStreamAttributes attrs = xmlReader.attributes();
                version = attrs.value("osisWork").toString();
                new_db = DBHelper::createBibleDB(version,lang);

                continue;
            }
            if (elementName == "div"){
                QString new_book_id = xmlReader.attributes().value("osisID").toString();
                if (new_book_id!=book_id){
                    if (osis_book.book_id!=""){
                        DBHelper::insertBook(osis_book, new_db);
                        book_count++;
                        // Once we're done waiting, value is updated
                        emit valueChanged(book_count);
                    }
                    book_id = new_book_id;
                    osis_book = Book(book_id);
                    osis_book.version = version;
                }
            }
            if (elementName == "chapter"){
                c_num = xmlReader.attributes().value("osisID").toInt();
            }
            if (elementName == "verse") {
                v_num = xmlReader.attributes().value("osisID").toInt();
                ref = QString(book_id+".%1.%2").arg(c_num).arg(v_num);
                ref = ref.split(".")[0]+"."+ref.split(".")[1]+":"+ref.split(".")[2];
                text = xmlReader.readElementText(QXmlStreamReader::SkipChildElements);
                //qDebug() << ref;
                verse = Verse(text,"",ref);
                verse.version = version;
                osis_book.addVerse(verse);
            }
        }
    }

    if (osis_book.book_id!="") {                  //insert the last book
        DBHelper::insertBook(osis_book, new_db);
        book_count++;
        emit valueChanged(book_count);
    }
    new_db.close();
    xmlReader.clear();
    xmlFile.close();

    // Set _working to false, meaning the process can't be aborted anymore.
    this->mutex.lock();
    this->_working = false;
    this->mutex.unlock();

    qDebug()<<"Worker process finished in Thread "<<thread()->currentThreadId();

    //Once 60 sec passed, the finished signal is sent
    emit finished();
}*/






void Worker::doWork()
{
    qDebug()<<"Starting worker process in Thread "<<thread()->currentThreadId();

    //Parse the file
    QFile xmlFile(this->xmlfilepath);
    if (!xmlFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file " << this->xmlfilepath << " for reading";
    }
    QXmlStreamReader xmlReader(&xmlFile);

    //QString type;
    QString ref;
    QString text;
    QString version;
    Book osis_book=Book();
    Verse verse;
    QString book_id = "";
    QSqlDatabase new_db;

    int book_count=0;
    while(!xmlReader.atEnd()) {
        // Checks if the process should be aborted
        this->mutex.lock();
        bool abort = this->_abort;
        this->mutex.unlock();
        if (abort) {
            qDebug()<<"Aborting worker process in Thread "<<thread()->currentThreadId();
            break;
        }

        xmlReader.readNext();

        if (xmlReader.isStartDocument()) {
            continue;
        }

        if (xmlReader.isEndDocument()) {
            break;
        }

        if (xmlReader.isStartElement()){
            QString elementName = xmlReader.name().toString();
            if (elementName == "work"){
                QXmlStreamAttributes attrs = xmlReader.attributes();
                version = attrs.value("osisWork").toString();
                new_db = DBHelper::createBibleDB(version,lang);

                continue;
            }
            if (elementName == "verse") {
                ref = xmlReader.attributes().value("osisID").toString();
                QString new_book_id = ref.split(".")[0];
                if (new_book_id!=book_id){
                    //TODO Write the osis_book to database
                    if (osis_book.book_id!=""){
                        DBHelper::insertBook(osis_book, new_db);
                        book_count++;
                        // Once we're done waiting, value is updated
                        emit valueChanged(book_count);
                    }
                    book_id = new_book_id;
                    osis_book = Book(book_id);
                    osis_book.version = version;
                }
                ref = ref.split(".")[0]+"."+ref.split(".")[1]+":"+ref.split(".")[2];
                text = xmlReader.readElementText(QXmlStreamReader::SkipChildElements);
                //qDebug() << ref;
                verse = Verse(text,"",ref);
                verse.version = version;
                osis_book.addVerse(verse);
            }
        }
    }

    if (osis_book.book_id!="") {                  //insert the last book
        DBHelper::insertBook(osis_book, new_db);
        book_count++;
        emit valueChanged(book_count);
    }
    new_db.close();
    xmlReader.clear();
    xmlFile.close();

    // Set _working to false, meaning the process can't be aborted anymore.
    this->mutex.lock();
    this->_working = false;
    this->mutex.unlock();

    qDebug()<<"Worker process finished in Thread "<<thread()->currentThreadId();

    //Once 60 sec passed, the finished signal is sent
    emit finished();
}
