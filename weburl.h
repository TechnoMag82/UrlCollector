#ifndef WEBURL_H
#define WEBURL_H

#include <QString>
#include <QSet>
#include <QList>

class weburl
{
	public:
        weburl(bool favorite = false,
               const QString &link = "",
               const QString &info = "");
        ~weburl();
		bool isFavorite() const { return infavorite; }
		void setFavorite(bool fav) { infavorite = fav; }
		QString link() const { return inlink; }
		void setLink(const QString &title) { inlink = title; }
		QString info() const { return ininfo; }
		void setInfo(const QString &linkinfo) { ininfo = linkinfo; }
        void addTag(QString *tag);
        void removeTag(QString *tag);
        bool containsTag(const QString &tag);
        bool tagsCount() {
            if (tags == nullptr) {
                return 0;
            }
            return tags->size();
        }
        QList<QString*> * getTags() const { return tags; }

	private:
		QString inlink;
		QString ininfo;
        QList<QString*> *tags = nullptr;
		bool infavorite;
};

#endif
