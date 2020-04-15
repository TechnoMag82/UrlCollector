#include "addurl.h"

AddUrl::AddUrl(QWidget *parent, bool edit, int curRow)
	: QDialog(parent)
{
	setWindowTitle(tr("Add new URL"));
	setFixedSize(411, 202);
	buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setGeometry(QRect(50, 160, 341, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
     connect(buttonBox, SIGNAL(accepted()), this, SLOT(OkButton()));
     connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
     
    weburl = new QLineEdit(this);
    weburl->setObjectName(QString::fromUtf8("lineEdit"));
    weburl->setGeometry(QRect(10, 30, 391, 29));
    weburl->setFocus();
    infourl = new QTextEdit(this);
    infourl->setObjectName(QString::fromUtf8("textEdit"));
    infourl->setGeometry(QRect(10, 80, 391, 71));
    infourl->setTabChangesFocus(true);
    infourl->setWordWrapMode(QTextOption::WordWrap);
    chkFavorite = new QCheckBox(tr("Is favorite!"), this);
    chkFavorite->setGeometry(QRect(10, 160, 100, 18));
    label = new QLabel(this);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(10, 10, 57, 19));
    label->setText(tr("URL:"));
    label_2 = new QLabel(this);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(QRect(10, 60, 170, 19));
    label_2->setText(tr("Informtion about URL:"));
    if (edit==true && curRow !=-1)
    {
    	MainWindow * mw = static_cast<MainWindow*>(parent);
    	infourl->setPlainText(mw->classUrl[curRow]->info());
    	weburl->setText(mw->classUrl[curRow]->link());
    	if (mw->classUrl[curRow]->isFavorite()==true)
    	   	chkFavorite->setCheckState(Qt::Checked);
	}
}

void AddUrl::OkButton()
{
	if (weburl->text().length()!=0)
	{
		if (weburl->text().startsWith("http://") || weburl->text().startsWith("www."))
			accept();
		else
			QMessageBox::warning(this, tr("add Url"),
			tr("Web-Link is not correct!\n Link must start with \"http://\" or \"www.\""), QMessageBox::Ok);
	}	
	else
	{	QMessageBox::warning(this, tr("add Url"),
		tr("You must write web-link to cite!"), QMessageBox::Ok);
		weburl->setFocus();
	}
}
