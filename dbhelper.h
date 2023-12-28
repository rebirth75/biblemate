#ifndef DBHELPER_H
#define DBHELPER_H
#include <QSqlDatabase>
#include <QString>
#include "book.h"
#include "note.h"

class DBHelper
{
public:
    DBHelper();
    DBHelper(QString path_bibles, QString path_note, QString version);
    int getNumChapters(QString book_id);
    int getNumVerses(QString book_id, QString chapter);
    Book GetBook(QString vers, QString book_id);
    std::vector<Note> getNotes(QString book_id);
    std::vector<Note> getAllNotes(bool ribbon);
    std::vector<Note> getAllNotes(QStringList texts, bool ribbon);
    std::vector<Note> getAllRibbons();
    std::vector<Verse> searchText(QStringList texts, bool exact);
    void createBibleTable();
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
