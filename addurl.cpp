#include "addurl.h"

AddUrl::AddUrl(QWidget *parent, bool edit, weburl *url)
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
     
    editWeburl = new QLineEdit(this);
    editWeburl->setObjectName(QString::fromUtf8("lineEdit"));
    editWeburl->setGeometry(QRect(10, 30, 391, 29));
    editWeburl->setFocus();
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
    if (edit == true)
    {
        infourl->setPlainText(url->info());
        editWeburl->setText(url->link());
        if (url->isFavorite() == true)
    	   	chkFavorite->setCheckState(Qt::Checked);
	}
}

void AddUrl::OkButton()
{
    if (editWeburl->text().length() != 0)
	{
        if (editWeburl->text().startsWith("http://") ||
                editWeburl->text().startsWith("www.") ||
                editWeburl->text().startsWith("https://")) {
			accept();
        } else {
            QMessageBox::warning(this,
                                 tr("add Url"),
                                 tr("Web-Link is not correct!"),
                                 QMessageBox::Ok);
        }
    } else {
        QMessageBox::warning(this,
                             tr("add Url"),
                             tr("You must write web-link to cite!"),
                             QMessageBox::Ok);
        editWeburl->setFocus();
	}
}
