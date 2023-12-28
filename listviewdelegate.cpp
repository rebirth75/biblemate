#include "listviewdelegate.h"
#include <QColor>

QSize ListViewDelegate::iconSize = QSize(10, 10);
int ListViewDelegate::padding = 1;
//int fontsize, titlesize;

ListViewDelegate::ListViewDelegate(int fs, int ts)
{
    ListViewDelegate::fontsize=fs;
    ListViewDelegate::titlesize=ts;
}

ListViewDelegate::~ListViewDelegate()
{
}

QSize ListViewDelegate::sizeHint(const QStyleOptionViewItem &  option ,
                                        const QModelIndex & index) const
{
    if(!index.isValid())
        return QSize();

    QString headerText = index.data(HeaderRole).toString();
    QString subheaderText = index.data(SubheaderRole).toString();

    QFont headerFont = QApplication::font();

    if (headerText.toStdString()=="\n")
        headerFont.setPixelSize(1); //PROVA
    else
        headerFont.setPointSize(titlesize);

    headerFont.setBold(true);
    QFont subheaderFont = QApplication::font();
    subheaderFont.setPointSize(fontsize);
    QFontMetrics headerFm(headerFont);
    QFontMetrics subheaderFm(subheaderFont);

    /* No need for x,y here. we only need to calculate the height given the width.
     * Note that the given height is 0. That is because boundingRect() will return
     * the suitable height if the given geometry does not fit. And this is exactly
     * what we want.
     */
    QRect headerRect = headerFm.boundingRect(0, 0,
                                             option.rect.width() - iconSize.width(), 0,
                                             Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap,
                                             headerText);

    QString verse = subheaderText.split(" ")[0];
    QString text_no_verse = subheaderText.remove(verse+" ");

    QRect subheaderRect1 = subheaderFm.boundingRect(0, 0,
                                                   30, 0,
                                                   Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap,
                                                   verse);
    QRect subheaderRect2 = subheaderFm.boundingRect(0, 0,
                                                   option.rect.width() - iconSize.width()-30, 0,
                                                   Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap,
                                                   text_no_verse);

    int height;
    //if (headerText.toStdString()!="")
        height = headerRect.height() + subheaderRect2.height() +  3*padding;
    //else
    //    height = subheaderRect.height() +  3*padding;

    QSize size(option.rect.width(), height);

    /* Keep the minimum height needed in mind. */
    if(size.height()<iconSize.height())
        size.setHeight(iconSize.height());

    return size;
}

void ListViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())
        return;

    painter->save();

    if (option.state & QStyle::State_Selected){
        //painter->fillRect(option.rect, option.palette.highlight());
        painter->fillRect(option.rect, QColor(255,255,0,125));
    }

    QString headerText = index.data(HeaderRole).toString();
    QString subheaderText = index.data(SubheaderRole).toString();

    QFont headerFont = QApplication::font();

    if (headerText.toStdString()=="\n")
        headerFont.setPixelSize(1); //PROVA
    else
        headerFont.setPointSize(titlesize);

    headerFont.setBold(true);
    QFont subheaderFont = QApplication::font();
    subheaderFont.setPointSize(fontsize);
    QFont verseFont = QApplication::font();
    verseFont.setPointSize(10);
    QFontMetrics headerFm(headerFont);
    QFontMetrics subheaderFm(subheaderFont);

    /*
     * The x,y coords are not (0,0) but values given by 'option'. So, calculate the
     * rects again given the x,y,w.
     * Note that the given height is 0. That is because boundingRect() will return
     * the suitable height if the given geometry does not fit. And this is exactly
     * what we want.
     */
    QRect headerRect =
            headerFm.boundingRect(option.rect.left() + iconSize.width(), option.rect.top() + padding,
                                  option.rect.width() - iconSize.width(), 0,
                                  Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap,
                                  headerText);

    QString verse = subheaderText.split(" ")[0];
    QString text_no_verse = subheaderText.remove(verse+" ");

    QRect subheaderRect1 =
            subheaderFm.boundingRect(headerRect.left(), headerRect.bottom()+padding,
                                     30, 0,
                                     Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap,
                                     verse);

    QRect subheaderRect2 =
            subheaderFm.boundingRect(headerRect.left()+30, headerRect.bottom()+padding,
                                     option.rect.width() - iconSize.width()-30, 0,
                                     Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap,
                                     text_no_verse);

    painter->setPen(Qt::black);

    painter->setFont(headerFont);
    painter->drawText(headerRect, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, headerText);

    painter->setFont(verseFont);
    painter->drawText(subheaderRect1, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, verse);

    painter->setFont(subheaderFont);
    painter->drawText(subheaderRect2, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, text_no_verse);

    painter->restore();
}
