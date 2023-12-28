#include "verse.h"
#include <QStringList>

Verse::Verse(QString text, QString title, QString ref)
{
    this->text=text;
    this->title=title;
    QStringList temp = ref.split("\\.");
    this->book = temp[0];
    this->chap = (temp[1].toInt());
    this->verse_num = (temp[2].toInt());
}
Verse::Verse(QString version)
{
    this->version=version;
}

QString Verse::getRef(){
    QString ref;
    ref=this->book+"."+QString("%1:%2").arg(this->chap).arg(this->verse_num);
    return ref;
}
