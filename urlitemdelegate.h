#ifndef URLITEMDELEGATE_H
#define URLITEMDELEGATE_H

#include <QObject>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QRect>
#include <QImage>

#include "linkstructure.h"
#include "weburl.h"

class UrlItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    public:
        UrlItemDelegate(QObject* pobj = 0);
        ~UrlItemDelegate();
        void paint(QPainter* painter,
                           const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;
        QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
        void setLinks(LinkStructure *linkStructure);
    private:
        LinkStructure *linkStructure = nullptr;
        QImage *favImage = nullptr;
        QImage *favImage1 = nullptr;
        QImage *linkImage = nullptr;

        void drawCaption(QPainter *painter, QRect const &rect, int row) const;
        void drawLink(QPainter *painter, QRect const &rect, int row) const;
};

#endif // URLITEMDELEGATE_H
