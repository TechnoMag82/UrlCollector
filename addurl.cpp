#include "addurl.h"

AddUrl::AddUrl(QWidget *parent, int editedRow, LinkStructure *linkStructure)
    : QDialog(parent)
{
    setWindowTitle(tr("Add new URL"));
    setMinimumWidth(500);

    this->editedRow = editedRow;
    this->linkStructure = linkStructure;

    QVBoxLayout *vboxLayout = new QVBoxLayout();

    editWeburl = new QLineEdit(this);
    editWeburl->setObjectName(QString::fromUtf8("lineEdit"));
    editWeburl->setFocus();

    infourl = new QTextEdit(this);
    infourl->setObjectName(QString::fromUtf8("textEdit"));
    infourl->setTabChangesFocus(true);
    infourl->setWordWrapMode(QTextOption::WordWrap);
    infourl->setAcceptRichText(false);

    QLabel *label = new QLabel(this);
    label->setObjectName(QString::fromUtf8("label"));
    label->setText(tr("URL:"));

    vboxLayout->addWidget(label);
    vboxLayout->addWidget(editWeburl);

    label = new QLabel(this);
    label->setObjectName(QString::fromUtf8("label_2"));
    label->setText(tr("Informtion about URL:"));

    vboxLayout->addWidget(label);
    vboxLayout->addWidget(infourl);

    label = new QLabel(this);
    label->setObjectName(QString::fromUtf8("label_3"));
    label->setText(tr("Tags:"));

    tagsList = new QListWidget(this);
    tagsList->setObjectName(QString::fromUtf8("tagsList"));
    tagsList->setMaximumHeight(120);

    vboxLayout->addWidget(label);
    vboxLayout->addWidget(tagsList);

    QHBoxLayout *hboxLayout = new QHBoxLayout();

    editNewTag = new QLineEdit(this);
    editNewTag->setObjectName(QString::fromUtf8("editNewTag"));
    editNewTag->setPlaceholderText(tr("Name of new tag"));

    buttonAddTag = new QPushButton(this);
    buttonAddTag->setObjectName(QString::fromUtf8("addTag"));
    buttonAddTag->setToolTip(tr("Press for add new tag"));
    buttonAddTag->setText("+");
    buttonAddTag->setMaximumWidth(32);
    connect(buttonAddTag, SIGNAL(clicked()), this, SLOT(addTag()));

    hboxLayout->addWidget(editNewTag);
    hboxLayout->addWidget(buttonAddTag);
    vboxLayout->addLayout(hboxLayout);

    chkFavorite = new QCheckBox(tr("Is favorite!"), this);

    vboxLayout->addWidget(chkFavorite);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(OkButton()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    vboxLayout->addWidget(buttonBox);

    setLayout(vboxLayout);

    if (editedRow != -1)
    {
        infourl->setPlainText(linkStructure->urlAt(editedRow)->info());
        editWeburl->setText(linkStructure->urlAt(editedRow)->link());
        if (linkStructure->isFavoriteLink(editedRow) == true) {
            chkFavorite->setCheckState(Qt::Checked);
        }
    }
    if (editedRow != -1 && linkStructure->urlAt(editedRow) != nullptr) {
        initTags(linkStructure->urlAt(editedRow)->getTags());
    } else {
        initTags(nullptr);
    }
}

weburl *AddUrl::getUrl()
{
    weburl *url = nullptr;
    url = linkStructure->urlAt(editedRow);
    if (url == nullptr) {
        url = linkStructure->addUrl(chkFavorite->isChecked(), editWeburl->text(), infourl->toPlainText());
    } else {
        url->setLink(editWeburl->text());
        url->setInfo(infourl->toPlainText());
        url->setFavorite(chkFavorite->isChecked());
    }
    fillTags(url);
    return url;
}

void AddUrl::OkButton()
{
    if (editWeburl->text().length() == 0)
    {
        QMessageBox::warning(this,
                             tr("Add Url"),
                             tr("You must write web-link to cite!"),
                             QMessageBox::Ok);
        editWeburl->setFocus();
        return;
    }
    if (linkStructure->isLinkExists(editWeburl->text())) {
        QMessageBox::warning(this,
                             tr("add Url"),
                             tr("Web-Link is exsists in DB!"),
                             QMessageBox::Ok);
        editWeburl->setFocus();
        return;
    }
    if (editWeburl->text().startsWith("http://") ||
            editWeburl->text().startsWith("www.") ||
            editWeburl->text().startsWith("https://")) {
        accept();
    } else {
        QMessageBox::warning(this,
                             tr("add Url"),
                             tr("Web-Link is not correct!"),
                             QMessageBox::Ok);
        editWeburl->setFocus();
    }
}

void AddUrl::addTag()
{
    QString strTag = editNewTag->text();
    if (strTag.isEmpty()) {
        QMessageBox::warning(this,
                             tr("Add Tag"),
                             tr("Cannot add empty tag!"),
                             QMessageBox::Ok);
        return;
    }
    if (strTag.contains(','))  {
        QMessageBox::warning(this,
                             tr("Add Tag"),
                             tr("Tag cannot contain character ','!"),
                             QMessageBox::Ok);
        return;
    }
    QString *tag = new QString(strTag);
    linkStructure->appendTagToMainList(tag);
    addTagItem(tag, Qt::Checked);
    editNewTag->clear();
}

void AddUrl::initTags(QList<QString*> *urltags)
{
    int allTagsCount = linkStructure->allTagsCount();
    for (int i = 0; i < allTagsCount; i++) {
        QListWidgetItem *newItem = addTagItem(linkStructure->getAllTags()->at(i), Qt::Unchecked);
        if (urltags != nullptr && editedRow != -1) {
            int count = urltags->size();
            for (int j = 0; j < count; j++) {
                // отмечаем тэги, которые присутсвуют в ссылке
                if (urltags->at(j)->compare(linkStructure->getAllTags()->at(i)) == 0) {
                    newItem->setCheckState(Qt::Checked);
                }
            }
        }
    }
}

// обновляем список тэгов в ссылке
void AddUrl::fillTags(weburl *url)
{
    int count = linkStructure->allTagsCount();
    if (count == 0) {
        return;
    }
    QList<QString*> *tags = url->getTags();
    if (tags != nullptr) {
        tags->clear();
    }
    for (int i = 0; i < count; i++) {
        if (tagsList->item(i)->checkState() == Qt::Checked) {
                url->addTag(linkStructure->getAllTags()->at(i));
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
