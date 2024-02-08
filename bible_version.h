#ifndef BIBLE_VERSION_H
#define BIBLE_VERSION_H

#include <QString>

class Bible_version
{
public:
    Bible_version();
    Bible_version(QString ln, QString nm, QString ver, QString fn, QString ic):lang(ln),name(nm),version(ver), filename(fn),icon(ic){};
    QString lang, name, version, filename, icon;
};

#endif // BIBLE_VERSION_H
