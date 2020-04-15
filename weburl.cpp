#include "weburl.h"

weburl::weburl(bool favorite, const QString &link, const QString &info)
{
	infavorite = favorite;
	inlink = link;
	ininfo = info;
}

