

#include "optionsdialog.h"
//#include "mainwindow.h"

OptionsDialog::OptionsDialog(QWidget *parent)
	: QDialog(parent)
{
	setFixedSize(377, 184);
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setGeometry(QRect(20, 130, 341, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(okButton()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    browse1 = new QPushButton(this);
    browse1->setObjectName(QString::fromUtf8("browse1"));
    browse1->setGeometry(QRect(280, 30, 80, 29));
    connect(browse1, SIGNAL(clicked()), this, SLOT(browseFile()));
    
    browse2 = new QPushButton(this);
    browse2->setObjectName(QString::fromUtf8("brose2"));
    browse2->setGeometry(QRect(280, 80, 80, 29));
    connect(browse2, SIGNAL(clicked()), this, SLOT(browseFile()));
    
    editDefBrowser = new QLineEdit(this);
    editDefBrowser->setObjectName(QString::fromUtf8("editDefBrowser"));
    editDefBrowser->setGeometry(QRect(10, 30, 251, 29));
    editDefBrowser->setReadOnly(true);
    editPathToDB = new QLineEdit(this);
    editPathToDB->setObjectName(QString::fromUtf8("editPathToDB"));
    editPathToDB->setGeometry(QRect(10, 80, 251, 29));
    editPathToDB->setReadOnly(true);
    label = new QLabel(this);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(10, 10, 181, 19));
    label_2 = new QLabel(this);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(QRect(10, 60, 241, 19));
    
    setWindowTitle(tr("Options"));
    browse1->setText(tr("Browse ..."));
    browse2->setText(tr("Browse ..."));
    label->setText(tr("Default web-browser:"));
    label_2->setText(tr("Path to database:"));
    
//    MainWindow * mw = static_cast<MainWindow*>(parent);
//    editDefBrowser->setText(mw->strDefBrowser);
//    editPathToDB->setText(mw->strPathToDB);
}

void OptionsDialog::browseFile()
{
	if (this->sender()==browse2)
		editPathToDB->setText(QFileDialog::getSaveFileName(this, tr("Select file of BataBase"), "/home", "Data Base Of URL's (*.ucl)"));
	if (this->sender()==browse1)
		editDefBrowser->setText(QFileDialog::getOpenFileName(this, tr("Select default browser"), "/home", "All files (*)"));
}

void OptionsDialog::okButton()
{
	if (editDefBrowser->text().length()!=0 && editPathToDB->text().length()!=0)
	{
		if (QFile::exists(editPathToDB->text())==false) // если файл не существует то создаем пустую базу
			{
				QFile file(editPathToDB->text());
				file.open(QIODevice::WriteOnly | QIODevice::Text);
				QMessageBox::information(this, "Options.", "Created new DataBese!", QMessageBox::Ok);
			}
		accept();
	}
		else
		QMessageBox::warning(this, tr("Edit program Options.") ,
		tr("You must select default web-browser and path to database of URL's!"),
		QMessageBox::Ok);
}
