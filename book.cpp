#include "book.h"

#include <QStringList>

Book::Book()
{

}

Book::Book(QString book_id)
{
    this->book_id=book_id;

}
void Book::addVerse(Verse v)
{
    this->verses.push_back(v);
    if (this->verses.size() > 1){
        Verse last = this->verses.at(this->verses.size()-2);
        if (this->num_cap!=last.chap) this->num_cap++;
     }
}
Verse* Book::getVerse(int cap, int v)
{
    for (Verse verse:this->verses){
        if (verse.chap==cap&&verse.verse_num==v){
            return &verse;
        }
    }
    return NULL;
}
QStringList Book::getStringList()
{
    QStringList stringList;

    int cap=0;
    for (Verse newVerse:this->verses){
        if (newVerse.chap!=cap) {

            QString temp = QString::fromStdString("\nCap."+ std::to_string(newVerse.chap));
            stringList.append(temp);
            temp = QString::fromStdString(std::to_string(newVerse.verse_num) + " " + newVerse.text.toStdString());
            stringList.append(temp);
            cap=newVerse.chap;
        }
        else {
            QString temp = QString::fromStdString(std::to_string(newVerse.verse_num) + " " + newVerse.text.toStdString());
            stringList.append(temp);
        }
    }
    return stringList;
}
