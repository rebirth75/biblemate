#ifndef BOOK_H
#define BOOK_H
#include "verse.h"
#include <vector>

class Book
{
public:
    Book()=default;
    Book(QString id):book_id(id),num_cap(0){};

    std::vector<Verse> verses;
    QString book_id="";
    int num_cap=0;

    void addVerse(Verse v);
    QStringList getStringList();

private:

};

#endif // BOOK_H
