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

#include "weburl.h"
#include "mainwindow.h"

class AddUrl : public QDialog
{
    Q_OBJECT
	public:
        AddUrl(QWidget *parent = 0, bool edit=false, weburl *url = nullptr, QList<QString*> *allTags = nullptr);
		// edit=true открыт диалог для редактирвания ссылки, edit=false - для добавления ссылки
        // weburl - структура, которую собираемся изменить
        QString webUrl() const { return editWeburl->text(); }
        QString infoUrl() const { return infourl->toPlainText(); }
        bool isFavorite() const { return chkFavorite->isChecked(); }
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

        weburl *url = nullptr;
        QList<QString*> *allTags = nullptr;
        bool edit = false;

        void initTags(QList<QString*> *urltags);
        void fillTags(weburl *Url);
};

#endif
