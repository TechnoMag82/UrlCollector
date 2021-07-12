#ifndef LINKSTRUCTURE_H
#define LINKSTRUCTURE_H

#include <QList>
#include <QTextStream>
#include <QFile>

#include "weburl.h"

class LinkStructure
{
    public:
        LinkStructure();
        ~LinkStructure();

        QList<QString*>* getAllTags() { return allTags; }

        bool mainListContainsTag(const QString &tag);
        void clearAllTags();
        void clearUrlList();
        bool isFavoriteLink(int i);
        int urlsCount();
        int urlTagsCount(int i);
        int allTagsCount();

        void removeUrlAt(int i);
        void removeTagAt(int i);
        void renameTagAt(int i, QString tagName);

        void appendTagToMainList(QString *tag);

        weburl *urlAt(int i);
        weburl* addUrl(bool favorite, QString link, QString info);
        void addTagToUrl(weburl *url, QString *tag);
        QList<QString*> * getTagsOfUrl(int i);
        void saveDB(QString pathToDb);
        QString tagsAt(int i);

    private:
        QList<weburl*> *listUrl = nullptr;
        QList<QString*> *allTags = nullptr;
};

#endif // LINKSTRUCTURE_H
