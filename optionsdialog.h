#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_h

#include <QDialog>
#include <QtGui>

#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>

class OptionsDialog : public QDialog
{
	Q_OBJECT
	public:
		OptionsDialog(QWidget *parent = 0);
		QLineEdit *editDefBrowser;
    	QLineEdit *editPathToDB;
	private slots:
		void okButton();
		void browseFile();
	private:
		QDialogButtonBox *buttonBox;
    	QPushButton *browse1;
    	QPushButton *browse2;
    	QLabel *label;
    	QLabel *label_2;
};

#endif
