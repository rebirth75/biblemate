#ifndef LISTVIEWDELEGATE_4_H
#define LISTVIEWDELEGATE_4_H

#include <QStyledItemDelegate>
#include <QLabel>
#include <QPainter>
#include <QApplication>

class ListViewDelegate_4 : public QStyledItemDelegate
{
public:
    enum datarole { HeaderRole = Qt::UserRole + 100, SubheaderRole};

    ListViewDelegate_4(int fontsize, int titlesize);
    ~ListViewDelegate_4();

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index ) const;

    static QSize iconSize;
    static int padding;
    int fontsize, titlesize;
};

#endif // LISTVIEWDELEGATE_4_H
