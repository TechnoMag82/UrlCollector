#include "mainwindow.h"

const QString PROGRAM_NAME="URLCollector v1.5";
const QString PROGRAM_DIR="/.urlcol";
const QString PROGRAM_CONFIG="/.urlcol/url.config";

MainWindow::MainWindow()
{
	setWindowTitle( PROGRAM_NAME );
	setWindowIcon(QIcon(":/images/mainwindow.png"));
    homeDir = QDir::homePath();
	setWindowState(Qt::WindowActive);
	
    search = new QLineEdit(this);
    search->setClearButtonEnabled(true);
    search->setPlaceholderText(tr("Enter text to find link"));
    search->setFixedWidth(400);
    search->setToolTip(tr("Find URL by info (Ctrl+F)"));
    connect(search, SIGNAL(textEdited(QString)), this, SLOT(searchInDB(QString)));
	
    urlListWidget = new QListWidget(this);
    urlListWidget->setSpacing(1);
    urlListWidget->setSelectionRectVisible(true);
    urlListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(urlListWidget, SIGNAL(itemClicked(QListWidgetItem *)), this , SLOT(getInfo(QListWidgetItem *)));
    connect(urlListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this , SLOT(gotoUrl()));
    setCentralWidget(urlListWidget);
	
	createActions();
	createMenu();
	createToolBar();
	createStatusBar();
	createDocWindows();
    createTagsPopupMenu();
	QTimer::singleShot(0, this, SLOT(initApp()));
    connect(tagListWidget, SIGNAL(customContextMenuRequested(QPoint)),
                   SLOT(customMenuRequested(QPoint)));
    initMonitoringClipboard();
}

MainWindow::~MainWindow()
{
    clearAllTags();
    clearUrlList();
    delete listUrl;
    delete allTags;
    delete rootTagsItem;
}

void MainWindow::initApp()
{
    readSettings();
}

void MainWindow::createDatabase()
{
    QString pathToNewDB = QFileDialog::getSaveFileName(this, tr("Select file of DataBase"), "/home", "Data Base Of URL's (*.ucl)");
    if (!pathToNewDB.isEmpty()) {
        if (!pathToNewDB.endsWith(".ucl")) {
            pathToNewDB.append(".ucl");
        }
        strPathToDB = pathToNewDB;
        if (QFile::exists(strPathToDB) == false) // если файл не существует то создаем пустую базу
        {
            QFile file(strPathToDB);
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            file.close();
        }
        saveSettings();
        loadDB();
        QMessageBox::information(this,
                                 tr("Options"),
                                 tr("Created new DataBese!"),
                                 QMessageBox::Ok);
    }
}

void MainWindow::clipboardChanged()
{
    if (boolMonitoringClipboard == false) {
        return;
    }

    if(board->text().isEmpty()) {
        return;
    }

    if (oldClipboard != board->text() &&
        (board->text().startsWith("https://") ||
         board->text().startsWith("http://") ||
         board->text().startsWith("www.")
        ))
    {
        qDebug() << "datachanged:" <<  board->text();
        oldClipboard = board->text();
        _addItem(new weburl(false, board->text(), tr("Added automatically")));
        dataEdited = true;
    }
}

void MainWindow::renameTag()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Rename tag"),
                                               tr("New tag name:"), QLineEdit::Normal,
                                               *allTags->at(selectedTagIndex), &ok);
    if (ok == true && !text.isEmpty()) {
        QString *tag = allTags->at(selectedTagIndex);
        // переписываем данные по адресу, на который указывает указатель
        tag->setRawData(text.data(), text.size());
        selectedTagItem->setText(0, text);
        dataEdited = true;
    }
}

void MainWindow::deleteTag()
{
    QString *tag = allTags->at(selectedTagIndex);
    int ret = QMessageBox::question(this,
                                  tr("Delete tag"),
                                  QString(tr("Delete tag '%1' from all links?")).arg(*tag),
                                  QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        allTags->removeAt(selectedTagIndex);
        rootTagsItem->removeChild(selectedTagItem);
        int count = listUrl->size();
        for (int i = 0; i < count; i++) {
            weburl *url = listUrl->at(i);
            if (url->containsTag(*tag)) {
                url->getTags()->removeAll(tag);
            }
        }
        delete tag;
        dataEdited = true;
    }
}

void MainWindow::customMenuRequested(QPoint pos)
{
    if (popupMenuTags != nullptr) {
        selectedTagItem = tagListWidget->itemAt(pos);
        QModelIndex item = tagListWidget->indexAt(pos);
        selectedTagIndex = item.row();
        popupMenuTags->popup(tagListWidget->viewport()->mapToGlobal(pos));
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (dataEdited == true) // спрашиваем о сохранении базы если были изменения
	{
        int ret = QMessageBox::question(this,
                                      tr("Quit from program"),
                                      tr("DataBase has been modified. Save DataBase of URL's?"),
                                      QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::Yes)
			saveDB();
	}
}

void MainWindow::selectBrowser(QStringList args)
{
    QString browser = QFileDialog::getOpenFileName(this, tr("Select browser"), ".", "All files (*)");
    if (browser.isEmpty() == false)
        QProcess::startDetached(browser, args);
}

void MainWindow::resetList()
{
    if (isSearching == true) {
        search->setText("");
        search->clear();
        int count = listUrl->size();
        for (int i = 0; i < count; i++) // пройдемся посписку
        {
            urlListWidget->item(i)->setHidden(false);
        }
        isSearching = false;
    }
}

void MainWindow::gotoUrl() // открываем браузер с ссылкой
{
    if (urlListWidget->count() != 0 || urlListWidget->currentRow() != -1)
	{
		QStringList args;
        args << listUrl->at(urlListWidget->currentRow())->link();
        if (!strDefBrowser.isEmpty() && (
                this->sender() == actOpenUrl ||
                this->sender() == actToolGoToUrl ||
                this->sender() == urlListWidget)) // если хотим открыть в браузере по-умолчанию
        {
			QProcess::startDetached(strDefBrowser, args);
        } else {
            selectBrowser(args);
            return;
        }
        if (this->sender() == actOpenUrlWith) {// выберем другой браузер (не в терминале)
            selectBrowser(args);
        }
    } else {
        QMessageBox::warning(this,
                             tr("Open link."),
                             tr("List is empty"),
                             QMessageBox::Ok);
    }
}

void MainWindow::searchInDB(const QString text) // метод поиска первого эл-та с подстрокой text
{
    if (strPathToDB.isEmpty())
        return;
    if (text.isEmpty()) {
        resetList();
    }
    if (text.length() < 3) {
        return;
    }
    int count = listUrl->size();
    for (int i = 0; i < count; i++) // пройдемся посписку
    {
        QListWidgetItem *item = urlListWidget->item(i);
        if (!text.isEmpty() && (
            listUrl->at(i)->link().indexOf(text, 0, Qt::CaseInsensitive) != -1 ||
            listUrl->at(i)->info().indexOf(text, 0, Qt::CaseInsensitive) != -1))
        {
            item->setHidden(false);
        } else {
            item->setHidden(true);
        }
    }
    isSearching = true;
}

void MainWindow::setSearchFocus()
{
	search->setFocus();
}

void MainWindow::createActions()
{
    actNewDatabase = new QAction(tr("New database ..."), this);
    actNewDatabase->setShortcut(tr("Ctrl+N"));
    actNewDatabase->setStatusTip(tr("Create new database"));
    connect(actNewDatabase, SIGNAL(triggered()), this, SLOT(createDatabase()));

    actOpenUrl = new QAction(QIcon(":/images/go-jump.png"),tr("Open URL"), this);
    actOpenUrl->setShortcut(tr("Ctrl+O"));
    actOpenUrl->setStatusTip(tr("Oprn link in web-browser."));
    connect(actOpenUrl, SIGNAL(triggered()), this, SLOT(gotoUrl()));

    actOpenUrlWith = new QAction(tr("Open URL with ..."), this);
    actOpenUrlWith->setShortcut(tr("Alt+O"));
    actOpenUrlWith->setStatusTip(tr("Open link in other web-browser."));
    connect(actOpenUrlWith, SIGNAL(triggered()), this, SLOT(gotoUrl()));

    actAddUrl = new QAction(QIcon(":/images/list-add.png"), tr("Add URL ..."), this);
    actAddUrl->setShortcut(tr("Ctrl+A"));
    actAddUrl->setStatusTip(tr("Add new URL in list."));
    connect(actAddUrl, SIGNAL(triggered()), this, SLOT(execAddUrl()));

    actToolAddUrl= new QAction(QIcon(":/images/toolbar/list-add.png"), tr("Add URL ..."), this);
    actToolAddUrl->setStatusTip(tr("Add new URL in list."));
    connect(actToolAddUrl, SIGNAL(triggered()), this, SLOT(execAddUrl()));

    actToolDelUrl= new QAction(QIcon(":/images/toolbar/list-remove.png"), tr("Delete URL ..."), this);
    actToolDelUrl->setStatusTip(tr("Delete URL from list."));
    connect(actToolDelUrl, SIGNAL(triggered()), this, SLOT(delUrl()));

    actToolGoToUrl = new QAction(QIcon(":/images/toolbar/go-jump.png"), tr("Open URL with default web-browser..."), this);
    actToolGoToUrl->setStatusTip(tr("Open URL in with default web-browser."));
    connect(actToolGoToUrl, SIGNAL(triggered()), this, SLOT(gotoUrl()));

    actToolFavorite = new QAction(QIcon(":/images/toolbar/emblem-favorite.png"), tr("Show only favorite links."), this);
    actToolFavorite->setStatusTip(tr("Show only favorite links from base."));
    actToolFavorite->setCheckable(true);
    connect(actToolFavorite, SIGNAL(triggered()), this, SLOT(showFavorites()));

    actToolEditUrl = new QAction(QIcon(":/images/toolbar/accessories-text-editor.png"), tr("Edit data about URL ..."), this);
    actToolEditUrl->setStatusTip(tr("Edit data about URL."));
    connect(actToolEditUrl, SIGNAL(triggered()), this, SLOT(execAddUrl()));

    actEditUrl = new QAction(QIcon(":/images/accessories-text-editor.png"), tr("Edit URL ..."), this);
    actEditUrl->setShortcut(tr("Ctrl+E"));
    actEditUrl->setStatusTip(tr("Edit data about URL."));
    connect(actEditUrl, SIGNAL(triggered()), this, SLOT(execAddUrl()));

    actDelUrl = new QAction(QIcon(":/images/list-remove.png"), tr("Delete URL"), this);
    actDelUrl->setShortcut(tr("Ctrl+D"));
    actDelUrl->setStatusTip(tr("Delete URL from list."));
    connect(actDelUrl, SIGNAL(triggered()), this , SLOT(delUrl()));

    actSearchUrl = new QAction(this); // при нажатии на Ctrl+F активирует строку ввода для поиска ссылок
    actSearchUrl->setShortcut(tr("Ctrl+F"));
    connect(actSearchUrl, SIGNAL(triggered()), this, SLOT(setSearchFocus()));

    actExit = new QAction(QIcon(":/images/application-exit.png"), tr("Exit"), this);
    actExit->setShortcut(tr("Alt+X"));
    actExit->setStatusTip(tr("Close program."));
    connect(actExit, SIGNAL(triggered()), SLOT(close()));

    actOptions = new QAction(tr("Options ..."), this);
    actOptions->setShortcut(tr("Ctrl+P"));
    actOptions->setStatusTip(tr("program options."));
    connect(actOptions, SIGNAL(triggered()), SLOT(Options()));

    actAbout = new QAction(QIcon(":/images/help-about.png"), tr("About ..."), this);
    actAbout->setStatusTip(tr("About URL Collector."));
    connect(actAbout, SIGNAL(triggered()), this, SLOT(About()));
}

void MainWindow::readSettings() // считываем настройки программы из конфига
{
    if (QFile::exists(homeDir + PROGRAM_CONFIG))
	{
        QFile textFile(homeDir + PROGRAM_CONFIG);
		textFile.open(QIODevice::ReadOnly| QIODevice::Text);
		QTextStream inText(&textFile);
        strDefBrowser = inText.readLine(0);
        strPathToDB = inText.readLine(0);
        QString strMonitoringClipboard = inText.readLine(0);
        boolMonitoringClipboard = strMonitoringClipboard == "1";
        loadDB();
    } else {
		Options(); // если конфига нет, то открываем диалог настроек программы
    }
}

void MainWindow::saveSettings() // сохраняем настройки программы в конфиг
{
    QFile textFile(homeDir + PROGRAM_CONFIG);
    textFile.open(QIODevice::WriteOnly| QIODevice::Text| QIODevice::Truncate);
    QTextStream outText(&textFile);
    outText.setCodec("UFT-8");
    outText << strDefBrowser << endl;
    outText << strPathToDB << endl;
    outText << boolMonitoringClipboard << endl;
    dataEdited = false;
}

void MainWindow::Options() // открываем диалог настройек
{
    OptionsDialog *optionsDialog = new OptionsDialog(this, strDefBrowser, strPathToDB, boolMonitoringClipboard);
    if (optionsDialog->exec() == QDialog::Accepted)
	{
        strDefBrowser = optionsDialog->defaultBrowser();
        strPathToDB = optionsDialog->pathToDb();
        boolMonitoringClipboard = optionsDialog->monitoringClipboard();
		QDir dir(homeDir);
        if (dir.exists(homeDir + PROGRAM_DIR)) {
			saveSettings();
        } else {
            dir.mkdir(homeDir + PROGRAM_DIR);
			saveSettings();
		}
        loadDB();
	}
    delete optionsDialog;
}

void MainWindow::getInfo(QListWidgetItem *item) // получаем информацию о выделенной ссылке
{
    urlInfo->setPlainText(listUrl->at(urlListWidget->row(item))->info());
}

void MainWindow::selectByTag(QTreeWidgetItem *treeItem, int column)
{
    if (strPathToDB.isEmpty())
        return;
    int count = listUrl->size();
    bool isAllTags = treeItem->text(0) == tr("All tags");
    for (int i = 0; i < count; i++)
    {
        QListWidgetItem *item = urlListWidget->item(i);
        if (isAllTags) {
            item->setHidden(false);
        } else {
            QString tag = treeItem->text(0);
            if (listUrl->at(i)->containsTag(tag)) {
                item->setHidden(false);
            } else {
                item->setHidden(true);
            }
        }
    }
}

void MainWindow::delUrl() // удаляем выделенную ссылку
{
    if (urlListWidget->currentRow() != -1 || urlListWidget->count() != 0)
	{	
        int ret = QMessageBox::question(this,
                                        tr("Delete URL"),
                                        tr("Do you want to delete this web-link?"),
                                        QMessageBox::Yes | QMessageBox::No);
		if (ret == QMessageBox::Yes)
		{
            int curRow = urlListWidget->currentRow();
            QListWidgetItem *curItem = urlListWidget->takeItem(curRow); // удаляем элемент из QListWidget
            urlListWidget->removeItemWidget(curItem); // удаляем элемент из QListWidgetItem
            listUrl->removeAt(curRow);
            updateWindowsTitle();
            dataEdited = true;
		}
    } else {
        QMessageBox::warning(this,
                             tr("Delete URL"),
                             tr("No selected items"),
                             QMessageBox::Ok);
    }
}

void MainWindow::clearUrlList()
{
    urlListWidget->clear();
    if (listUrl != nullptr) {
        QList<weburl*>::iterator it = listUrl->begin();
        for (; it != listUrl->end(); ++it) {
            delete *it;
        }
        listUrl->clear();
        for (int i = 0; i < listUrl->size(); i++) {
            delete listUrl->takeAt(i);
        }
    }
}

bool MainWindow::containsTag(const QString &tag)
{
    if (allTags != nullptr) {
        QList<QString*>::iterator it = allTags->begin();
        for (; it != allTags->end(); ++it) {
            QString *aTag = *it;
            if (aTag->compare(tag) == 0) {
                return true;
            }
        }
    }
    return false;
}

bool MainWindow::treeContainTag(const QString &tag)
{
    int count = rootTagsItem->childCount();
    for (int i = 0; i < count; i++) {
        QTreeWidgetItem *widgetItem = rootTagsItem->child(i);
        if (widgetItem->text(0).compare(tag) == 0) {
            return true;
        }
    }
    return false;
}

void MainWindow::updateTags(weburl *url)
{
    QList<QString*> *tags = url->getTags();
    if (tags == nullptr) {
        return;
    }
    int count = tags->size();
    for (int i = 0; i < count; i++) {
        QString *aTag  = tags->at(i);
        if (!treeContainTag(*aTag)) {
            addTagWidgetItem(*aTag);
        }
    }
}

void MainWindow::updateWindowsTitle()
{
    QString temp;
    setWindowTitle(PROGRAM_NAME + " [" + QFileInfo(strPathToDB).fileName() + "] - elements in DB " + temp.setNum(listUrl->count(), 10) );
}

void MainWindow::showFavorites() // скрываем/показываем все избранные ссылки
{
    if (strPathToDB.isEmpty())
        return;
    QListWidgetItem *item = new QListWidgetItem;
    int count = listUrl->size();
    for (int i = 0; i < count; i++) // пройдемся посписку
    {
        if (listUrl->at(i)->isFavorite() == false) // если эл-т не фаворит
        {
            item = urlListWidget->item(i); // получаем нужный элемент (и его свойства)
            if (actToolFavorite->isChecked() == true) // и если кнопка тулбара нажаьа
                item->setHidden(true); // скрываем его
            else // если кнопка улбара не нажата
                item->setHidden(false);	// показываем его
        }
    }
}

bool MainWindow::loadDB()
{
	QFile textDB(strPathToDB);
    if (textDB.open(QIODevice::ReadOnly | QIODevice::Text) == true)
	{
        clearAllTags();
        clearUrlList();
        if (listUrl == nullptr) {
            listUrl = new QList<weburl*>();
        }
		// временные строки для хранения инфы
		QString temp;
		QString strLink;
		QString strInfo;
        QString strTags;
        QList<QString*> *tempTags = new QList<QString*>();
        bool favorite = false; // инициализируем эту ф-ню
		
        qDebug() << "Loading Database ...";
		QTextStream inDB(&textDB);
		inDB.setCodec("UTF-8");
        addRootTreeItem();
        while (inDB.atEnd() != true) // пока не дочитали до конца файла
		{
            temp = inDB.readLine(0); // читаем строку
            if (temp.startsWith("link: ")) // если встретилась ссылка
                strLink = temp.remove(0, 6); // сохраняем ссылку

            if (temp.startsWith("status: normal")) // если встретился статус
                favorite = false;

            if (temp.startsWith("status: favorite")) // если встретился статус
                favorite = true;

            if (temp.startsWith("tags: ")) {
                if (allTags == nullptr) {
                    allTags = new QList<QString*>();
                }
                strTags = temp.remove(0, 5);
                QStringList spTags = strTags.split(',', QString::SkipEmptyParts);
                QStringList::iterator it = spTags.begin();
                for (; it != spTags.end(); ++it) {
                    QString name = *it;
                    QString *aTag = new QString(name.remove(0, 1));
                    tempTags->append(aTag);
                    if (!containsTag(*aTag)) {
                        allTags->append(aTag);
                        addTagWidgetItem(*aTag);
                    }
                }
                tagListWidget->expandAll();
            }

            if (temp.startsWith("info: ")) // если начался блок текста с информацией о ссылке
            {
                temp = inDB.readLine(0);
                strInfo = temp;
                while (inDB.atEnd() != true){
                    temp = inDB.readLine(0);
                    if (temp.startsWith("endinfo.") == true) // если кончился болк текста с инфой
                        break;
                    strInfo += "\n";
                    strInfo += temp;
                }
                weburl *url = new weburl(favorite, strLink, strInfo);
                QList<QString*>::iterator it = tempTags->begin();
                for (; it != tempTags->end(); ++it) {
                    url->addTag(*it);
                }
                tempTags->clear();
                _addItem(url);	// добавляем ссылку
            }
		} // end while
        textDB.close();
        updateWindowsTitle();
        qDebug() << "Database is loaded.";
        dataEdited = false;
		return true;
    } else {
        qDebug() << "Database is not loaded. Can't open file. " << strPathToDB;
        return false;
	}
}

void MainWindow::saveDB() // сохраняем базу ссылок
{
	QFile textDB(strPathToDB);
    if (textDB.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate) == true)
	{
		QTextStream outDB(&textDB); // создаем поток вывода текста в файл
		outDB.setCodec("UTF-8");
        int count = listUrl->size();
        for (int i = 0; i < count; i++) // пройдемся поо БД
		{
            outDB << "link: " << listUrl->at(i)->link() << endl;
            if (listUrl->at(i)->isFavorite() == true)
				outDB << "status: favorite" << endl;
			else
				outDB << "status: normal" << endl;
            int tagsCount = listUrl->at(i)->tagsCount();
            if (tagsCount > 0) {
                outDB << "tags: ";
                QList<QString*> *tags = listUrl->at(i)->getTags();
                for (int j = 0; j < tagsCount; j++) {
                    outDB << *tags->at(j);
                    if (j < tagsCount - 1) {
                         outDB << ", ";
                    }
                }
                outDB << endl;
            }
            outDB << "info: " << endl << listUrl->at(i)->info() << endl << "endinfo." << endl;
		}
        textDB.close();
	}
}

void MainWindow::_addItem(weburl *url)
// добавление эл-та в базу
// bool gui - длбавляем и в список и в память?
{
    addWidgetItem(url->isFavorite(), url->link());
    listUrl->append(url);
}

void MainWindow::addTagWidgetItem(const QString &tag)
{
    QTreeWidgetItem *newItem = new QTreeWidgetItem();
    newItem->setText(0, tag);
    newItem->setBackground(0, QBrush(randomColor(), Qt::SolidPattern));
    rootTagsItem->addChild(newItem);
}

void MainWindow::setItemFavorite(bool favorite, QListWidgetItem *newItem) // настрока вида элемента
{
    if (favorite == true) {
        newItem->setBackground(QBrush(QColor(249, 251, 205), Qt::SolidPattern));
        newItem->setIcon(QIcon(":/images/bookmark-new.png"));
    } else {
        newItem->setIcon(QIcon(":/images/text-html.png"));
        newItem->setBackground(QBrush(QColor(205, 240, 251), Qt::SolidPattern));
    }
}

void MainWindow::initMonitoringClipboard()
{
    board = QGuiApplication::clipboard();
    connect(board, &QClipboard::dataChanged, this, &MainWindow::clipboardChanged, Qt::DirectConnection);
}

void MainWindow::addRootTreeItem()
{
    if (rootTagsItem == nullptr) {
        rootTagsItem = new QTreeWidgetItem();
        rootTagsItem->setText(0, tr("All tags"));
        tagListWidget->addTopLevelItem(rootTagsItem);
    } else {
        rootTagsItem->takeChildren().clear();
    }
}

QColor MainWindow::randomColor()
{
    QString uuid = QUuid::createUuid().toString();
    bool ok;
    int r = uuid.mid(1, 2).toInt(&ok, 16);
    int g = uuid.mid(3, 2).toInt(&ok, 16);
    int b = uuid.mid(5, 2).toInt(&ok, 16);
    if (r < 100) {
        r += 150;
    }
    if (g < 100) {
        g += 150;
    }
    if (b < 100) {
        b += 150;
    }
    return QColor(r, g, b);
}

void MainWindow::clearAllTags()
{
    if (allTags != nullptr) {
        QList<QString*>::iterator it = allTags->begin();
        for (; it != allTags->end(); ++it) {
            delete *it;
        }
        delete allTags;
        allTags = nullptr;
    }
    if (rootTagsItem != nullptr) {
        while(rootTagsItem->childCount() != 0)
            delete rootTagsItem->takeChild(0);
    }
}

QTreeWidgetItem *MainWindow::getRoot() const
{
    return rootTagsItem;
}

void MainWindow::setRoot(QTreeWidgetItem *value)
{
    rootTagsItem = value;
}

void MainWindow::addWidgetItem(bool favorite, QString text) // добавляем виджет эелмента в список
{
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(text);
    setItemFavorite(favorite, newItem);
    urlListWidget->addItem(newItem);
}

void MainWindow::addItemToList() // добавляем ссылку в список
{
    if (!strPathToDB.isEmpty()) {
        resetList(); // вернемся к исходному списку
        AddUrl *addUrlDialog = new AddUrl(this, false, nullptr, allTags);
        if (addUrlDialog->exec() == QDialog::Accepted)
        {
            weburl *url = addUrlDialog->getUrl();
            _addItem(url);
            allTags = addUrlDialog->getAllTags();
            updateTags(url);
            updateWindowsTitle();
            dataEdited = true;
        }
        delete addUrlDialog;
    } else {
        QMessageBox::critical(this,
                              tr("Edit database"),
                              tr("Database is not opened!"),
                              QMessageBox::Ok);
    }
}

void MainWindow::refreshItem() // обновляем измененный элемент списка
{
    if (urlListWidget->count() != 0) {
        weburl *url = listUrl->at(urlListWidget->currentRow());
        AddUrl *addUrlDialog = new AddUrl(this, true, url, allTags);
        if (addUrlDialog->exec() == QDialog::Accepted)
		{
            url = addUrlDialog->getUrl();
            allTags = addUrlDialog->getAllTags();
            updateTags(url);
            QListWidgetItem *curItem = urlListWidget->currentItem();
            curItem->setText(url->link());
            setItemFavorite(url->isFavorite(), curItem);
            urlInfo->setPlainText(url->info());
            dataEdited = true;
		}
        delete addUrlDialog;
    } else {
        QMessageBox::critical(this,
                              tr("Edit data about URL."),
                              tr("List is empty!"),
                              QMessageBox::Ok);
    }
}

void MainWindow::execAddUrl() // вызов диалога добавления ссылки
{
    if (this->sender() == actAddUrl || this->sender() == actToolAddUrl) // если диалог вызвается из меню "Добавить адрес"
		addItemToList();
    if (this->sender() == actEditUrl || this->sender() == actToolEditUrl) // если диалог вызвается из меню "Редактировать адрес"
		refreshItem();
}

void MainWindow::createToolBar()
{
	mainToolBar = addToolBar(tr("Main"));
	mainToolBar->addAction(actToolAddUrl);
	mainToolBar->addAction(actToolDelUrl);
	mainToolBar->addAction(actToolEditUrl);
	mainToolBar->addSeparator();
	mainToolBar->addAction(actToolFavorite);
	mainToolBar->addSeparator();
	mainToolBar->addWidget(search);
	mainToolBar->addAction(actToolGoToUrl);
	search->addAction(actSearchUrl);
}

void MainWindow::createMenu()
{
    menuUrl = menuBar()->addMenu(tr("File"));
        menuUrl->addAction(actNewDatabase);
        menuUrl->addSeparator();
		menuUrl->addAction(actOpenUrl);
		menuUrl->addAction(actOpenUrlWith);
		menuUrl->addAction(actAddUrl);
		menuUrl->addAction(actEditUrl);
		menuUrl->addAction(actDelUrl);
		menuUrl->addSeparator();
		menuUrl->addAction(actExit);
		
	menuProgram = menuBar()->addMenu(tr("Program"));
		menuProgram->addAction(actOptions);
		
	menuHelp = menuBar()->addMenu(tr("Help"));	
		menuHelp->addAction(actAbout);
}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Welcome !!!"));
}

void MainWindow::createDocWindows()
{
    QDockWidget *plainTextDoc = new QDockWidget(tr("URL information"), this);
	plainTextDoc->setAllowedAreas(Qt::BottomDockWidgetArea);
	urlInfo = new QTextEdit(plainTextDoc);
	plainTextDoc->setWidget(urlInfo);
	plainTextDoc->setFeatures(QDockWidget::NoDockWidgetFeatures);
	plainTextDoc->setMaximumHeight(170);
	addDockWidget(Qt::BottomDockWidgetArea, plainTextDoc);
	urlInfo->setReadOnly(true);

    QDockWidget *tagsDoc = new QDockWidget(tr("Tags"), this);
    tagsDoc->setAllowedAreas(Qt::LeftDockWidgetArea);
    tagListWidget = new QTreeWidget(tagsDoc);
    tagListWidget->header()->close();
    tagsDoc->setWidget(tagListWidget);
    tagsDoc->setFeatures(QDockWidget::NoDockWidgetFeatures);
    tagsDoc->setMinimumWidth(170);
    tagsDoc->setMaximumWidth(270);
    addDockWidget(Qt::LeftDockWidgetArea, tagsDoc);
    connect(tagListWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this , SLOT(selectByTag(QTreeWidgetItem *, int)));
}

void MainWindow::createTagsPopupMenu()
{
    tagListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    popupMenuTags = new QMenu(this);
    QAction *action = new QAction(tr("Rename tag"), this);
    popupMenuTags->addAction(action);
    connect(action, &QAction::triggered, this, &MainWindow::renameTag);
    action = new QAction(tr("Delete tag"), this);
    popupMenuTags->addAction(action);
    connect(action, &QAction::triggered, this, &MainWindow::deleteTag);
}

void MainWindow::About()
{
	QMessageBox::about(this, "About " + PROGRAM_NAME,
	"<font size=14>About " + PROGRAM_NAME +"</font>"
	"<p>program for collect web-links</p>"
	"<br>Created by TechnoMag (C) 2009");
}
