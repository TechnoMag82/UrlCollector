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
		AddUrl( QWidget *parent = 0, bool edit=false, int curRow=-1);
		// edit=true открыт диалог для редактирвания ссылки, edit=false - для добавления ссылки
		// curRow - текущая выделенная ссылка в списке
	public:
		QLineEdit *weburl;
		QTextEdit *infourl;
		QCheckBox *chkFavorite;
	private slots:
		void OkButton();
	private:	
		QDialogButtonBox *buttonBox;
		QLabel *label;
		QLabel *label_2;
};

#endif
