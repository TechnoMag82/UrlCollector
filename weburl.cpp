#include "weburl.h"

weburl::weburl(bool favorite,
               const QString &link,
               const QString &info)
{
	infavorite = favorite;
	inlink = link;
    ininfo = info;
}

weburl::~weburl()
{
    tags->clear();
    delete tags;
}

void weburl::addTag(QString *tag)
{
    if (tags == nullptr) {
        tags = new QList<QString*>();
    }
    tags->append(tag);
}

void weburl::removeTag(QString *tag)
{
    if (tags != nullptr) {
        tags->removeAll(tag);
    }
}

bool weburl::containsTag(const QString &tag)
{
    if (tags != nullptr) {
        QList<QString*>::iterator it = tags->begin();
        for (; it != tags->end(); ++it) {
            QString *aTag = *it;
            if (aTag->compare(tag) == 0) {
                return true;
            }
        }
    }
    return false;
}
