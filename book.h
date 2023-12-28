#ifndef BOOK_H
#define BOOK_H
#include "verse.h"
#include <vector>

class Book
{
public:
    Book();
    Book(QString id);
    std::vector<Verse> verses;
    QString book_id;
    int num_cap;

    void addVerse(Verse v);
    Verse* getVerse(int cap,int v);
    QStringList getStringList();

private:

};

#endif // BOOK_H
