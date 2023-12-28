#include "note.h"

Note::Note()
{

}

QString Note::getRef(){
    QString ref;
    ref=this->book_id+"."+QString("%1:%2").arg(this->chap).arg(this->verse);
    return ref;
}
