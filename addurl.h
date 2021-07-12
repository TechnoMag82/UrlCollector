#ifndef ADDURL_H
#define ADDURL_H

#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QTextEdit>
#include <QLabel>
#include <QCheckBox>
#include <QMessageBox>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "weburl.h"
#include "mainwindow.h"
#include "linkstructure.h"

class AddUrl : public QDialog
{
    Q_OBJECT
	public:
        AddUrl(QWidget *parent = 0, int editedRow=-1, LinkStructure *linkStructure = 0);
        weburl *getUrl();
	private slots:
		void OkButton();
        void addTag();
    private:
		QDialogButtonBox *buttonBox;
        QLineEdit *editWeburl;
        QLineEdit *editNewTag;
        QPushButton *buttonAddTag;
        QTextEdit *infourl;
        QCheckBox *chkFavorite;
        QListWidget *tagsList;

        LinkStructure *linkStructure = nullptr;

        int editedRow = -1;

        void initTags(QList<QString*> *urltags);
        void fillTags(weburl *url);
        QListWidgetItem *addTagItem(QString *tag, Qt::CheckState isChecked);
};

#endif
