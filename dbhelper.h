#ifndef DBHELPER_H
#define DBHELPER_H
#include <QSqlDatabase>
#include <QString>
#include "book.h"
#include "note.h"
#include "bible_version.h"

class DBHelper
{
public:
    DBHelper();
    DBHelper(QString path_note, Bible_version);
    int getNumChapters(QString book_id);
    int getNumVerses(QString book_id, QString chapter);
    Book GetBook(QString vers, QString book_id);
    Verse GetVerse(QString vers, QString ref);
    std::vector<Note> getNotes(QString book_id);
    std::vector<Note> getAllNotes(bool ribbon);
    std::vector<Note> getAllNotes(QStringList texts, bool ribbon);
    std::vector<Note> getAllRibbons();
    std::vector<Verse> searchText(QStringList texts, bool exact);
    QSqlDatabase static createBibleDB(QString path,QString lang);
    void static createBibleTable(QSqlDatabase new_db);
    int static insertVerse(Verse verse, QSqlDatabase new_db);
    void static insertBook(Book book, QSqlDatabase new_db);
    void createNoteTable();
    void mergeBibleDbs(std::string version, std::string path);
    int insertNote(Note note);
    bool updateNote(Note note);
    bool deleteNote(Note note);
    Note findNote(QString ref);
private:
    QSqlDatabase m_db, m_db_notes;
};

#endif // DBHELPER_H
