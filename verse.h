#ifndef VERSE_H
#define VERSE_H
#include <QString>


class Verse
{
public:
    Verse()=default;
    Verse(QString text, QString title, QString ref);
    Verse(QString version):version(version){};
    QString getRef();

    QString text, book, title, version, link;
    int chap=0;
    int verse_num=0;

private:

};

#endif // VERSE_H
