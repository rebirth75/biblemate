#ifndef NOTE_H
#define NOTE_H
#include <QString>

class Note
{
public:
    Note();
    int id;
    QString book_id;
    int chap;
    int verse;
    QString title;
    QString text;
    QString note_date;
    QString hlight_color;
    bool ribbon;
    bool editing;
    QString ribbon_title;
    QString ribbon_date;
    QString getRef();

};

#endif // NOTE_H



