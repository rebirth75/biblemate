#include "verse.h"
#include <QStringList>

Verse::Verse(QString text, QString title, QString ref)
{
    this->text=text;
    this->title=title;
    QStringList temp = ref.split(".");
    this->book = temp[0];
    temp = temp[1].split(":");
    this->chap = (temp[0].toInt());
    this->verse_num = (temp[1].toInt());
}
/*Verse::Verse(QString version)
{
    this->version=version;
}*/

QString Verse::getRef(){
    QString ref;
    ref=this->book+"."+QString("%1:%2").arg(this->chap).arg(this->verse_num);
    return ref;
}

void Verse::setRef(QString ref){
    this->book=ref.split(".")[0];
    this->chap=ref.split(".")[1].split(":")[0].toInt();
    this->verse_num=ref.split(".")[1].split(":")[1].toInt();
}
