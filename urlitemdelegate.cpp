#include "urlitemdelegate.h"

UrlItemDelegate::UrlItemDelegate(QObject* pobj)
    : QStyledItemDelegate(pobj)
{
    favImage = new QImage(":/images/bookmark-new.png");
    favImage1 = new QImage(":/images/emblem-favorite.png");
    linkImage = new QImage(":/images/text-html.png");
}

UrlItemDelegate::~UrlItemDelegate()
{
    delete favImage;
    delete favImage1;
    delete linkImage;
}

void UrlItemDelegate::paint(QPainter *painter,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
            painter->save();

            if (option.state & QStyle::State_Selected) {
                painter->fillRect(option.rect, option.palette.highlight());
            } else if (linkStructure->urlAt(index.row())->isFavorite()) {
                painter->fillRect(option.rect, option.palette.midlight());
            } else {
                painter->fillRect(option.rect, option.palette.midlight());
            }

            if (linkStructure->urlAt(index.row())->isFavorite()) {
                painter->drawImage(QPoint(option.rect.left() + 8, option.rect.top() + 10), *favImage);
                painter->drawImage(QPoint(option.rect.right() - 32, option.rect.top() + 8), *favImage1);
            } else {
                painter->drawImage(QPoint(option.rect.left() + 8, option.rect.top() + 10), *linkImage);
            }

            QRect target(option.rect.left() + 36,
                         option.rect.top() + 8,
                         option.rect.width(),
                         option.rect.height());
            drawCaption(painter, target, index.row());

            target.setRect(option.rect.left() + 36,
                         option.rect.top() + 32,
                         option.rect.width(),
                         option.rect.height());
            drawLink(painter, target, index.row());
            painter->restore();
}

QSize UrlItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(option.rect.width(), 60);
}

void UrlItemDelegate::setLinks(LinkStructure *linkStructure)
{
    this->linkStructure = linkStructure;
}

void UrlItemDelegate::drawCaption(QPainter *painter, const QRect &rect, int row) const
{

    QFont font = painter->font();
    font.setPixelSize(18);
    painter->setFont(font);
    if (linkStructure->urlAt(row)->info().isEmpty()) {
        painter->setPen(QColor(169, 91, 95));
        painter->drawText(rect, tr("No description"));
    } else if (linkStructure->urlAt(row)->info().length() > 32) {
        painter->setPen(QColor(0, 0, 0));
        painter->drawText(rect, linkStructure->urlAt(row)->info().left(32) + "...");
    } else {
        painter->setPen(QColor(0, 0, 0));
        painter->drawText(rect, linkStructure->urlAt(row)->info());
    }
}

void UrlItemDelegate::drawLink(QPainter *painter, const QRect &rect, int row) const
{
    painter->setPen(QColor(148, 148, 148));
    QFont font = painter->font();
    font.setPixelSize(13);
    painter->setFont(font);
    if (linkStructure->urlAt(row)->link().length() > 64) {
        painter->drawText(rect, linkStructure->urlAt(row)->link().left(64) + "...");
    } else {
        painter->drawText(rect, linkStructure->urlAt(row)->link());
    }
}
