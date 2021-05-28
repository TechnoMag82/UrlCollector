#ifndef ADDURL_H
#define ADDURL_H

#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QTextEdit>
#include <QLabel>
#include <QCheckBox>
#include <QMessageBox>

#include "weburl.h"
#include "mainwindow.h"

class AddUrl : public QDialog
{
	Q_OBJECT	
	public:
        AddUrl( QWidget *parent = 0, bool edit=false, weburl *url = 0);
		// edit=true открыт диалог для редактирвания ссылки, edit=false - для добавления ссылки
        // weburl - структура, которую собираемся изменить
        QString webUrl() const { return editWeburl->text(); }
        QString infoUrl() const { return infourl->toPlainText(); }
        bool isFavorite() const { return chkFavorite->isChecked(); }
	private slots:
		void OkButton();
	private:	
		QDialogButtonBox *buttonBox;
		QLabel *label;
		QLabel *label_2;
        QLineEdit *editWeburl;
        QTextEdit *infourl;
        QCheckBox *chkFavorite;
};

#endif
