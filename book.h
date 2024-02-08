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
    QString version="";
    int num_cap=0;
    std::vector<int> chap_vers;

    void addVerse(Verse v);
    QStringList getStringList();    
    std::vector<Verse> getPassage(QString ref1, QString ref2);
    void setChap_Vers();

private:

};

#endif // BOOK_H
