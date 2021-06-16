#include "addurl.h"

AddUrl::AddUrl(QWidget *parent, bool edit, weburl *url, QList<QString*> *allTags)
	: QDialog(parent)
{
	setWindowTitle(tr("Add new URL"));
    setFixedSize(411, 445);

    this->edit = edit;
    this->url = url;
    this->allTags = allTags;
	buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setGeometry(QRect(50, 400, 341, 32));
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
    infourl->setGeometry(QRect(10, 80, 391, 120));
    infourl->setTabChangesFocus(true);
    infourl->setWordWrapMode(QTextOption::WordWrap);

    chkFavorite = new QCheckBox(tr("Is favorite!"), this);
    chkFavorite->setGeometry(QRect(10, 400, 150, 18));

    QLabel *label = new QLabel(this);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(10, 10, 57, 19));
    label->setText(tr("URL:"));

    label = new QLabel(this);
    label->setObjectName(QString::fromUtf8("label_2"));
    label->setGeometry(QRect(10, 60, 170, 19));
    label->setText(tr("Informtion about URL:"));

    label = new QLabel(this);
    label->setObjectName(QString::fromUtf8("label_3"));
    label->setGeometry(QRect(10, 210, 170, 19));
    label->setText(tr("Tags:"));

    tagsList = new QListWidget(this);
    tagsList->setObjectName(QString::fromUtf8("tagsList"));
    tagsList->setGeometry(10, 230, 391, 120);

    editNewTag = new QLineEdit(this);
    editNewTag->setObjectName(QString::fromUtf8("editNewTag"));
    editNewTag->setPlaceholderText(tr("Name of new tag"));
    editNewTag->setGeometry(QRect(10, 360, 340, 29));

    buttonAddTag = new QPushButton(this);
    buttonAddTag->setObjectName(QString::fromUtf8("addTag"));
    buttonAddTag->setToolTip(tr("Press for add new tag"));
    buttonAddTag->setText("+");
    buttonAddTag->setGeometry(QRect(365, 360, 30, 29));
    connect(buttonAddTag, SIGNAL(clicked()), this, SLOT(addTag()));

    if (edit == true)
    {
        infourl->setPlainText(url->info());
        editWeburl->setText(url->link());
        if (url->isFavorite() == true)
    	   	chkFavorite->setCheckState(Qt::Checked);
	}
    if (url != nullptr) {
        initTags(url->getTags());
    } else {
        initTags(nullptr);
    }
}

weburl *AddUrl::getUrl()
{
    if (url == nullptr) {
        url = new weburl(chkFavorite->isChecked(), editWeburl->text(), infourl->toPlainText());
    } else {
        url->setLink(editWeburl->text());
        url->setInfo(infourl->toPlainText());
        url->setFavorite(chkFavorite->isChecked());
    }
    fillTags();
    return url;
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
                             tr("Add Url"),
                             tr("You must write web-link to cite!"),
                             QMessageBox::Ok);
        editWeburl->setFocus();
    }
}

void AddUrl::addTag()
{
    if (editNewTag->text().isEmpty()) {
        QMessageBox::warning(this,
                             tr("Add Tag"),
                             tr("Cannot add empty tag!"),
                             QMessageBox::Ok);
        return;
    }
    QString *tag = new QString(editNewTag->text());
    if (allTags == nullptr) {
        allTags = new QList<QString*>();
    }
    allTags->append(tag);
    addTagItem(tag, Qt::Checked);
}

void AddUrl::initTags(QList<QString*> *urltags)
{
    if (allTags == nullptr) {
        return;
    }
    int count = allTags->size();
    for (int i = 0; i < count; i++) {
        QListWidgetItem *newItem = addTagItem(allTags->at(i), Qt::Unchecked);
        if (urltags != nullptr && edit) {
            int count = urltags->size();
            for (int j = 0; j < count; j++) {
                // отмечаем тэги, которые присутсвуют в ссылке
                if (urltags->at(j)->compare(allTags->at(i)) == 0) {
                    newItem->setCheckState(Qt::Checked);
                }
            }
        }
    }
}

// обновляем список тэгов в ссылке
void AddUrl::fillTags()
{
    if (allTags == nullptr || url == nullptr) {
        return;
    }
    int count = allTags->size();
    if (count == 0) {
        return;
    }
    QList<QString*> *tags = url->getTags();
    if (tags != nullptr) {
        tags->clear();
    }
    for (int i = 0; i < count; i++) {
        if (tagsList->item(i)->checkState() == Qt::Checked) {
                url->addTag(allTags->at(i));
        }
    }
}

QListWidgetItem* AddUrl::addTagItem(QString *tag, Qt::CheckState isChecked)
{
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setCheckState(isChecked);
    newItem->setText(*tag);
    tagsList->addItem(newItem);
    return newItem;
}
