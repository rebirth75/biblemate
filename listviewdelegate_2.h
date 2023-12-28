#ifndef LISTVIEWDELEGATE_2_H
#define LISTVIEWDELEGATE_2_H

#include <QStyledItemDelegate>
#include <QLabel>
#include <QPainter>
#include <QApplication>

class ListViewDelegate_2 : public QStyledItemDelegate
{
public:
    enum datarole { HeaderRole = Qt::UserRole + 100, SubheaderRole};

    ListViewDelegate_2(int fontsize, int titlesize);
    ~ListViewDelegate_2();

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index ) const;

    static QSize iconSize;
    static int padding;
    int fontsize, titlesize;
};

#endif // LISTVIEWDELEGATE_2_H
