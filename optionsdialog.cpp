#include "optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget *parent,
                             QString browser,
                             QString pathDb,
                             bool monitoringClipboard)
	: QDialog(parent)
{
    setWindowTitle(tr("Options"));
    setMinimumWidth(600);
    
    QVBoxLayout *vboxLayout = new QVBoxLayout();

    QLabel *label = new QLabel(this);
    label->setObjectName(QString::fromUtf8("label"));
    label->setText(tr("Default web-browser:"));

    vboxLayout->addWidget(label);

    QHBoxLayout *hboxLayout = new QHBoxLayout();

    editDefBrowser = new QLineEdit(this);
    editDefBrowser->setObjectName(QString::fromUtf8("editDefBrowser"));
    editDefBrowser->setReadOnly(true);
    editDefBrowser->setText(browser);

    browse1 = new QPushButton(this);
    browse1->setObjectName(QString::fromUtf8("browse1"));
    browse1->setText(tr("Browse ..."));
    connect(browse1, SIGNAL(clicked()), this, SLOT(browseFile()));

    hboxLayout->addWidget(editDefBrowser);
    hboxLayout->addWidget(browse1);
    vboxLayout->addLayout(hboxLayout);

    label = new QLabel(this);
    label->setObjectName(QString::fromUtf8("label_2"));
    label->setText(tr("Path to database:"));

    vboxLayout->addWidget(label);
    hboxLayout = new QHBoxLayout();

    editPathToDB = new QLineEdit(this);
    editPathToDB->setObjectName(QString::fromUtf8("editPathToDB"));
    editPathToDB->setReadOnly(true);
    editPathToDB->setText(pathDb);

    browse2 = new QPushButton(this);
    browse2->setObjectName(QString::fromUtf8("brose2"));
    browse2->setText(tr("Browse ..."));
    connect(browse2, SIGNAL(clicked()), this, SLOT(browseFile()));

    hboxLayout->addWidget(editPathToDB);
    hboxLayout->addWidget(browse2);
    vboxLayout->addLayout(hboxLayout);
    
    chkMonitoringClipboard = new QCheckBox(tr("Monitoring clipboard"), this);
    chkMonitoringClipboard->setToolTip(tr("Turn ON monotoring clipboard for automatically add URL to database"));
    chkMonitoringClipboard->setChecked(monitoringClipboard);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(okButton()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    vboxLayout->addWidget(chkMonitoringClipboard);
    vboxLayout->addWidget(buttonBox);

    setLayout(vboxLayout);
}

OptionsDialog::~OptionsDialog()
{
    delete chkMonitoringClipboard;
    delete buttonBox;
    delete browse1;
    delete browse2;
    delete editDefBrowser;
    delete editPathToDB;
}

void OptionsDialog::browseFile()
{
    if (this->sender() == browse2) {
        QString path = QFileDialog::getOpenFileName(this, tr("Select file of BataBase"), "/home", "Data Base Of URL's (*.ucl)");
        if (!path.isEmpty())
            editPathToDB->setText(path);
    }
    if (this->sender() == browse1) {
        QString path = QFileDialog::getOpenFileName(this, tr("Select default browser"), "/home", "All files (*)");
        if (!path.isEmpty())
            editDefBrowser->setText(path);
    }
}

void OptionsDialog::okButton()
{
    if (QFile::exists(editPathToDB->text()) == true) {
		accept();
    } else {
        QMessageBox::warning(this,
                             tr("Edit program Options"),
                             tr("You must select database!"),
                             QMessageBox::Ok);
    }
}
