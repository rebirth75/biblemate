#ifndef VERSE_H
#define VERSE_H
#include <QString>


class Verse
{
public:
    Verse(QString text, QString title, QString ref);
    Verse(QString version);
    QString getRef();
    QString text, book, title, version;
    int chap, verse_num;
private:

};

#endif // VERSE_H
