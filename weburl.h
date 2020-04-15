#ifndef WEBURL_H
#define WEBURL_H

#include <QString>

class weburl
{
	public:
		weburl(bool favorite=false, const QString &link="", const QString &info="");
		
		bool isFavorite() const { return infavorite; }
		void setFavorite(bool fav) { infavorite = fav; }
		QString link() const { return inlink; }
		void setLink(const QString &title) { inlink = title; }
		QString info() const { return ininfo; }
		void setInfo(const QString &linkinfo) { ininfo = linkinfo; }

	private:
		QString inlink;
		QString ininfo;
		bool infavorite;
};

#endif
