#ifndef LISTVIEWDELEGATE_H
#define LISTVIEWDELEGATE_H

#include <QStyledItemDelegate>
#include <QLabel>
#include <QPainter>
#include <QApplication>

class ListViewDelegate : public QStyledItemDelegate
{
public:
    enum datarole { HeaderRole = Qt::UserRole + 100, SubheaderRole};

    ListViewDelegate(int fontsize, int titlesize);
    ~ListViewDelegate();

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index ) const;

    static QSize iconSize;
    static int padding;
    int fontsize, titlesize;
};

#endif // LISTVIEWDELEGATE_H
