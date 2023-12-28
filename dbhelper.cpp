#include "dbhelper.h"
#include <QSqlQuery>
#include "book.h"
#include "note.h"
#include <QDebug>
#include <QSqlError>
#include <QFile>

DBHelper::DBHelper()
{

}

DBHelper::DBHelper(const QString path_bibles, const QString path_notes, QString version)
{

    m_db = QSqlDatabase::addDatabase("QSQLITE", "bible");
    m_db.setDatabaseName(path_bibles+"/"+version+".db");
    m_db_notes  = QSqlDatabase::addDatabase("QSQLITE", "notes");
    m_db_notes.setDatabaseName(path_notes+"/notes.db");

    if (!m_db.open())
    {
        qDebug() << "Error: connection with database bible failed";
    }
    else
    {
        qDebug() << "Database: connection with bible ok";
    }
    if (!m_db_notes.open())
    {
        qDebug() << "Error: connection with database notes failed";
    }
    else
    {
        qDebug() << "Database: connection with notes ok";
        this->createNoteTable();
    }
}

/**
 * Create the Bible Table if not exists
 *
 * @brief DBHelper::createBibleTable
 */
void DBHelper::createBibleTable()
{
    QSqlQuery sql(m_db);
    sql.prepare("CREATE TABLE IF NOT EXISTS bibles (id INTEGER PRIMARY KEY, version TEXT, book TEXT, chap INTEGER, verse INTEGER, text TEXT, title TEXT, link TEXT);");
    qDebug() << sql.exec();

}

/**
 * Create the Note Table if not exists
 *
 * @brief DBHelper::createNoteTable
 */
void DBHelper::createNoteTable()
{
    QSqlQuery sql(m_db_notes);
    sql.prepare("CREATE TABLE IF NOT EXISTS notes (id INTEGER PRIMARY KEY, book TEXT, chap INTEGER, verse INTEGER, title TEXT, text TEXT, note_date TEXT, highlight TEXT, ribbon INTEGER, ribbon_title TEXT, ribbon_date TEXT);");
    qDebug() << sql.exec();
}


/**
 * Get the book as vector of verses
 *
 * @brief DBHelper::GetBook
 * @param version
 * @param book_id
 * @return
 */
Book DBHelper::GetBook(QString version, QString book_id)
{
    Book book(book_id);
    QSqlQuery query(m_db);

    query.prepare("SELECT * FROM bibles WHERE version=:version AND book=:book_id;");
    query.bindValue(":version", version);
    query.bindValue(":book_id", book_id);

    qDebug() << query.exec();
    qDebug() << query.lastError();
    qDebug() << query.executedQuery();

    while (query.next())
    {
        QString stemp = query.value(1).toString();
        Verse verse(stemp);
        stemp = query.value(2).toString();
        verse.book=stemp;
        int itemp = query.value(3).toInt();
        verse.chap=itemp;
        itemp = query.value(4).toInt();
        verse.verse_num=itemp;
        stemp = query.value(5).toString();
        verse.text = stemp;
        stemp = query.value(6).toString();
        verse.title = stemp;
        book.addVerse(verse);
    }
    return book;
}

int DBHelper::getNumChapters(QString book_id){
    QSqlQuery query(m_db);

    query.prepare("SELECT COUNT(DISTINCT chap) FROM bibles WHERE book=:book_id;");
    query.bindValue(":book_id", book_id);
    qDebug() << query.exec();
    query.first();
    return query.value(0).toInt();
}

int DBHelper::getNumVerses(QString book_id, QString chap){
    QSqlQuery query(m_db);

    query.prepare("SELECT COUNT(DISTINCT verse) FROM bibles WHERE book=:book_id AND chap=:chap;");
    query.bindValue(":book_id", book_id);
    query.bindValue(":chap", chap);
    qDebug() << query.exec();
    query.first();
    return query.value(0).toInt();
}

std::vector<Note> DBHelper::getNotes(QString book_id) {
    std::vector<Note> result;
    QSqlQuery query(m_db_notes);
    query.prepare("SELECT * FROM notes WHERE book =:book_id;");
    query.bindValue(":book_id", book_id);
    qDebug() << query.exec();
    while (query.next())
    {
        Note n;
        n.id = query.value(0).toInt();
        n.book_id = query.value(1).toString();
        n.chap = query.value(2).toInt();
        n.verse = query.value(3).toInt();
        n.title = query.value(4).toString();
        n.text = query.value(5).toString();
        n.note_date = query.value(6).toString();
        n.ribbon = query.value(8).toBool();
        n.ribbon_title = query.value(9).toString();
        n.ribbon_date = query.value(10).toString();
        n.editing = false;

        result.push_back(n);
    }
    return result;
}

std::vector<Note> DBHelper::getAllNotes(bool ribbon) {
    std::vector<Note> result;
    QSqlQuery query(m_db_notes);
    query.prepare("SELECT * FROM notes;");
    qDebug() << query.exec();
    while (query.next())
    {
        Note n;
        n.id = query.value(0).toInt();
        n.book_id = query.value(1).toString();
        n.chap = query.value(2).toInt();
        n.verse = query.value(3).toInt();
        n.title = query.value(4).toString();
        n.text = query.value(5).toString();
        n.note_date = query.value(6).toString();
        n.ribbon = query.value(8).toBool();
        n.ribbon_title = query.value(9).toString();
        n.ribbon_date = query.value(10).toString();
        n.editing = false;

        if ((n.text!=""||n.title!="")||ribbon)
            result.push_back(n);

    }
    return result;
}

std::vector<Note> DBHelper::getAllRibbons() {
    std::vector<Note> result;
    QSqlQuery query(m_db_notes);
    query.prepare("SELECT * FROM notes WHERE ribbon=1;");
    qDebug() << query.exec();
    while (query.next())
    {
        Note n;
        n.id = query.value(0).toInt();
        n.book_id = query.value(1).toString();
        n.chap = query.value(2).toInt();
        n.verse = query.value(3).toInt();
        n.title = query.value(4).toString();
        n.text = query.value(5).toString();
        n.note_date = query.value(6).toString();
        n.ribbon = query.value(8).toBool();
        n.ribbon_title = query.value(9).toString();
        n.ribbon_date = query.value(10).toString();
        n.editing = false;

        result.push_back(n);

    }
    return result;
}

int DBHelper::insertNote(Note note){

    QSqlQuery sql(m_db_notes);
    sql.prepare("INSERT INTO notes (book, chap, verse, title, text, note_date, ribbon_title, ribbon_date, ribbon) "
                "VALUES(:book_id, :chap, :verse, :title, :text, :note_date, :ribbon_title, :ribbon_date, :ribbon)");
    sql.bindValue(":book_id", note.book_id);
    sql.bindValue(":chap", note.chap);
    sql.bindValue(":verse", note.verse);
    sql.bindValue(":title", note.title);
    sql.bindValue(":text", note.text);
    sql.bindValue(":note_date", note.note_date);
    sql.bindValue(":ribbon_date", note.ribbon_date);
    sql.bindValue(":ribbon_title", note.ribbon_title);
    sql.bindValue(":ribbon", note.ribbon);

    qDebug() << sql.exec();
    qDebug() << sql.lastError();
    qDebug() << sql.executedQuery();

    return  sql.lastInsertId().toInt();
}

bool DBHelper::updateNote(Note note){

    QSqlQuery sql(m_db_notes);
    sql.prepare("UPDATE notes SET title=:title, text=:text, note_date=:note_date, "
                "ribbon_title=:ribbon_title, ribbon_date=:ribbon_date, ribbon=:ribbon "
                "WHERE id=:id;");
    sql.bindValue(":title", note.title);
    sql.bindValue(":text", note.text);
    sql.bindValue(":note_date", note.note_date);
    sql.bindValue(":ribbon_date", note.ribbon_date);
    sql.bindValue(":ribbon_title", note.ribbon_title);
    sql.bindValue(":ribbon", note.ribbon);
    sql.bindValue(":id", note.id);

    qDebug() << sql.exec();
    qDebug() << sql.lastError();
    qDebug() << sql.executedQuery();

    return  true;
}

bool DBHelper::deleteNote(Note note){

    QSqlQuery sql(m_db_notes);
    sql.prepare("DELETE FROM notes WHERE id=:id;");
    sql.bindValue(":id", note.id);

    qDebug() << sql.exec();
    qDebug() << sql.lastError();
    qDebug() << sql.executedQuery();

    return  true;
}

Note DBHelper::findNote(QString ref){
    QString book_id = ref.split(".")[0];
    QString chap = ref.split(".")[1].split(":")[0];
    QString verse = ref.split(".")[1].split(":")[1];

    QSqlQuery query(m_db_notes);

    query.prepare("SELECT * FROM notes WHERE book=:book_id AND chap=:chap AND verse=:verse;");
    query.bindValue(":book_id", book_id);
    query.bindValue(":chap", chap);
    query.bindValue(":verse", verse);
    qDebug() << query.exec();
    Note n;
    while (query.next())
    {
        n.id = query.value(0).toInt();
        n.book_id = query.value(1).toString();
        n.chap = query.value(2).toInt();
        n.verse = query.value(3).toInt();
        n.title = query.value(4).toString();
        n.editing = false;
        n.text = query.value(5).toString();
        n.note_date = query.value(6).toString();
        n.ribbon = query.value(8).toBool();
        n.ribbon_title = query.value(9).toString();
        n.ribbon_date = query.value(10).toString();
    }
    return n;
}

/**
 * Search the texts (list of text) in the Bible, if exact is true, the texts list represents a phrase
 *
 * @brief DBHelper::searchText
 * @param texts
 * @param exact
 * @return
 */
std::vector<Verse> DBHelper::searchText(QStringList texts, bool exact)
{
    std::vector<Verse> results;
    QString sql="SELECT * FROM bibles WHERE text LIKE :text1";

    QSqlQuery query(m_db);

    if (exact){
        QString temp = texts[0];
        for (int i=1;i<texts.size();i++){
            temp+=" "+texts[i];
        }
        texts[0]=temp;
    }

    if (texts.length()>1&&!exact){
        for (int i=2; i<=texts.length(); i++){
            sql += QString(" AND text LIKE :text%1").arg(i);
        }
        query.prepare(sql);
        for (int i=0; i<texts.length();i++){
            QString temp=QString(":text%1").arg(i+1);
            query.bindValue(temp, QString("%%1%").arg(texts[i]));
        }
    }
    else{
        //query.prepare("SELECT * FROM bibles WHERE version='nuovadiodati' AND id=1;");
        query.prepare(sql);
        query.bindValue(":text1", QString("%%1%").arg(texts[0]));
    }
    qDebug() << query.exec();
    qDebug() << query.lastError();
    qDebug() << query.executedQuery();
    while (query.next())
    {
        QString stemp = query.value(1).toString();
        Verse verse(stemp);
        stemp = query.value(2).toString();
        verse.book=stemp;
        int itemp = query.value(3).toInt();
        verse.chap=itemp;
        itemp = query.value(4).toInt();
        verse.verse_num=itemp;
        stemp = query.value(5).toString();
        verse.text = stemp;
        stemp = query.value(6).toString();
        verse.title = stemp;
        results.push_back(verse);
    }

    return results;
}

std::vector<Note> DBHelper::getAllNotes(QStringList texts, bool ribbon) {
    std::vector<Note> result;
    QSqlQuery query(m_db_notes);
    QString sql="SELECT * FROM notes WHERE text LIKE :text1 OR title LIKE :text1";
    if (texts.length()>1){
        for (int i=2; i<=texts.length(); i++){
            sql += QString(" AND (text LIKE :text%1 OR title LIKE :text%1)").arg(i);
        }
        query.prepare(sql);
        for (int i=0; i<texts.length();i++){
            QString temp=QString(":text%1").arg(i+1);
            query.bindValue(temp, QString("%%1%").arg(texts[i]));
        }
    }
    else{
        query.prepare(sql);
        query.bindValue(":text1", QString("%%1%").arg(texts[0]));
    }

    qDebug() << query.exec();
    while (query.next())
    {
        Note n;
        n.id = query.value(0).toInt();
        n.book_id = query.value(1).toString();
        n.chap = query.value(2).toInt();
        n.verse = query.value(3).toInt();
        n.title = query.value(4).toString();
        n.text = query.value(5).toString();
        n.note_date = query.value(6).toString();
        n.ribbon = query.value(8).toBool();
        n.ribbon_title = query.value(9).toString();
        n.ribbon_date = query.value(10).toString();
        n.editing = false;

        if ((n.text!=""||n.title!="")||ribbon)
            result.push_back(n);

    }
    return result;
}
