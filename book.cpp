#include "book.h"

#include <QStringList>


void Book::addVerse(Verse v)
{
    this->verses.push_back(v);
    if (this->verses.size() > 1){
        Verse last = this->verses.at(this->verses.size()-2);
        if (this->num_cap!=last.chap) this->num_cap++;
     }
}

void Book::setChap_Vers(){
    int chap=0;
    for (int i=0;i<(int)this->verses.size();i++){
        if (this->verses.at(i).chap==chap){
            this->chap_vers.at(chap-1)++;
        }
        else{
            chap++;
            this->chap_vers.push_back(1);
        }
    }
}

std::vector<Verse> Book::getPassage(QString ref1, QString ref2){
    int idx1=0;
    int idx2=0;
    for (int i=0; i<(int)this->verses.size();i++){
        if (this->verses.at(i).getRef()==ref1)
            idx1=i;
        if (this->verses.at(i).getRef()==ref2)
            idx2=i;
    }
    std::vector<Verse>temp;
    for (int i=idx1; i<idx2+1;i++){
        temp.push_back(this->verses.at(i));
    }
    return temp;
}

QStringList Book::getStringList()
{
    QStringList stringList;

    int cap=0;
    for (const auto & newVerse:this->verses){
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
