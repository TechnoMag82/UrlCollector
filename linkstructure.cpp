#include "linkstructure.h"

LinkStructure::LinkStructure()
{
    listUrl = new QList<weburl*>();
    allTags = new QList<QString*>();
}

LinkStructure::~LinkStructure()
{
    clearAllTags();
    clearUrlList();
    delete listUrl;
    delete allTags;
    listUrl = nullptr;
    allTags = nullptr;
}

bool LinkStructure::mainListContainsTag(const QString &tag)
{
    if (allTags != nullptr) {
        QList<QString*>::iterator it = allTags->begin();
        for (; it != allTags->end(); ++it) {
            QString *aTag = *it;
            if (aTag->compare(tag) == 0) {
                return true;
            }
        }
    }
    return false;
}

void LinkStructure::clearAllTags()
{
    if (allTags != nullptr) {
        qDeleteAll(*allTags);
        allTags->clear();
    }
}

void LinkStructure::clearUrlList()
{
    if (listUrl != nullptr) {
        qDeleteAll(*listUrl);
        listUrl->clear();
    }
}

bool LinkStructure::isFavoriteLink(int i)
{
    return listUrl->at(i)->isFavorite();
}

int LinkStructure::urlsCount()
{
    return listUrl->size();
}

int LinkStructure::urlTagsCount(int i)
{
    return listUrl->at(i)->tagsCount();
}

int LinkStructure::allTagsCount()
{
    return allTags->size();
}

weburl *LinkStructure::urlAt(int i)
{
    if (i == -1 || i > listUrl->size()) {
        return nullptr;
    }
    return listUrl->at(i);
}

void LinkStructure::removeUrlAt(int i)
{
    listUrl->removeAt(i);
}

void LinkStructure::removeTagAt(int i)
{
    QString *tag = allTags->at(i);
    allTags->removeAt(i);

    int count = listUrl->size();
    for (int j = 0; j < count; j++) {
        weburl *url = listUrl->at(j);
        if (url->containsTag(*tag)) {
            url->getTags()->removeAll(tag);
        }
    }
    delete tag;
}

void LinkStructure::renameTagAt(int i, QString tagName)
{
    QString *tag = allTags->at(i);
    // переписываем данные по адресу, на который указывает указатель
    tag->setRawData(tagName.data(), tagName.size());
}

void LinkStructure::appendTagToMainList(QString *tag)
{
    allTags->append(tag);
}

weburl *LinkStructure::addUrl(bool favorite, QString link, QString info)
{
    weburl *url = new weburl(favorite, link, info);
    listUrl->append(url);
    return url;
}

void LinkStructure::addTagToUrl(weburl *url, QString *tag)
{
    if (!url->containsTag(*tag)) {
        url->addTag(tag);
    }
}

QList<QString *> *LinkStructure::getTagsOfUrl(int i)
{
    return listUrl->at(i)->getTags();
}

void LinkStructure::saveDB(QString pathToDb)
{
    QFile textDB(pathToDb);
    if (textDB.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate) == true)
    {
        QTextStream outDB(&textDB); // создаем поток вывода текста в файл
        outDB.setCodec("UTF-8");
        int count = urlsCount();
        for (int i = 0; i < count; i++) // пройдемся поо БД
        {
            outDB << "link: " << listUrl->at(i)->link() << endl;
            if (listUrl->at(i)->isFavorite() == true)
                outDB << "status: favorite" << endl;
            else
                outDB << "status: normal" << endl;
            int tagsCount = listUrl->at(i)->tagsCount();
            if (tagsCount > 0) {
                outDB << "tags: ";
                QList<QString*> *tags = getTagsOfUrl(i);
                for (int j = 0; j < tagsCount; j++) {
                    outDB << *tags->at(j);
                    if (j < tagsCount - 1) {
                        outDB << ", ";
                    }
                }
                outDB << endl;
            }
            outDB << "info: " << endl << listUrl->at(i)->info() << endl << "endinfo." << endl;
        }
        textDB.close();
    }
}

QString LinkStructure::tagsAt(int i)
{
    QList<QString*> *tags = listUrl->at(i)->getTags();
    QString strTags;
    if (tags != nullptr) {
        QList<QString*>::iterator it;
        for (it = tags->begin(); it != tags->end(); ++it) {
            strTags.append(*it);
            strTags.append(" ");
        }
    }
    return strTags;
}

QString LinkStructure::trimLink(const QString link)
{
    QString temp = link;
    if (temp.startsWith("https://")) {
        temp = temp.remove("https://");
    }
    if (temp.startsWith("www.")) {
        temp = temp.remove("www.");
    }
    if (temp.startsWith("http://")) {
        temp = temp.remove("http://");
    }
    if (temp.endsWith('/')) {
        int last = temp.lastIndexOf('/');
        temp = temp.remove(last, 1);
    }
    return temp;
}

bool LinkStructure::isLinkExists(QString link)
{
    QString originLink = trimLink(link);
    int count = listUrl->size();
    for (int i = 0; i < count; i++) {
        QString comparedLink = trimLink(listUrl->at(i)->link());
        if (comparedLink.compare(originLink) == 0) {
            return true;
        }
    }
    return false;
}
