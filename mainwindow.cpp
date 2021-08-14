#include "mainwindow.h"

const QString PROGRAM_NAME="URLCollector v1.6";
const QString PROGRAM_DIR="/.urlcol";
const QString PROGRAM_CONFIG="/.urlcol/url.config";

MainWindow::MainWindow()
{
    homeDir = QDir::homePath();
    linkStructure = new LinkStructure();

    setWindowTitle( PROGRAM_NAME );
    QIcon windowIcon = QIcon(":/images/mainwindow.png");
    setWindowIcon(windowIcon);
    setWindowState(Qt::WindowActive);

    systemTrayIcon = new QSystemTrayIcon(windowIcon, this);
    createTrayMenu();
    systemTrayIcon->setVisible(true);
    systemTrayIcon->setToolTip(PROGRAM_NAME);
    systemTrayIcon->show();

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
    initApp();
    connect(tagListWidget, SIGNAL(customContextMenuRequested(QPoint)),
                   SLOT(customMenuRequested(QPoint)));
    initMonitoringClipboard();
}

MainWindow::~MainWindow()
{
    clearAllTags();
    clearUrlList();
    delete linkStructure;
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
        weburl *url = linkStructure->addUrl(false, board->text(), tr("Added automatically"));
        addUrlItem(url);
        QString *aTag = new QString(tr("Added automatically"));
        if (!linkStructure->mainListContainsTag(*aTag)) {
            linkStructure->appendTagToMainList(aTag);
            addTagWidgetItem(*aTag);
        }
        linkStructure->addTagToUrl(url, aTag);
        dataEdited = true;
        systemTrayIcon->showMessage("Url Collector",
                                    QString(tr("Link %1 \nadded to databse from clipboard")).arg(board->text().left(32)),
                                    QIcon(":/images/text-html.png"),
                                    5000);
    }
}

void MainWindow::renameTag()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Rename tag"),
                                               tr("New tag name:"), QLineEdit::Normal,
                                               selectedTagItem->text(0), &ok);
    if (ok == true && !text.isEmpty()) {
        linkStructure->renameTagAt(selectedTagIndex, text);
        selectedTagItem->setText(0, text);
        dataEdited = true;
    }
}

void MainWindow::deleteTag()
{
    int ret = QMessageBox::question(this,
                                  tr("Delete tag"),
                                  QString(tr("Delete tag '%1' from all links?")).arg(selectedTagItem->text(0)),
                                  QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        rootTagsItem->removeChild(selectedTagItem);
        linkStructure->removeTagAt(selectedTagIndex);
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

void MainWindow::openApp()
{
    show();
    activateWindow();
    raise();
    setFocus();
    setWindowState(Qt::WindowState::WindowActive);
}

void MainWindow::quitApp()
{
    QApplication::quit();
}

void MainWindow::exitApp()
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
    quitApp();
}

void MainWindow::autosaveDB()
{
    if (autosaveInterval > 0) {
        qDebug() << "Save DB by timer";
        saveDB();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    hide();
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
        int count = linkStructure->urlsCount();
        for (int i = 0; i < count; i++)
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
        args << linkStructure->urlAt(urlListWidget->currentRow())->link();
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
    int count = linkStructure->urlsCount();
    for (int i = 0; i < count; i++)
    {
        QListWidgetItem *item = urlListWidget->item(i);
        if (!text.isEmpty() && (
            linkStructure->urlAt(i)->link().indexOf(text, 0, Qt::CaseInsensitive) != -1 ||
            linkStructure->urlAt(i)->info().indexOf(text, 0, Qt::CaseInsensitive) != -1 ||
            linkStructure->urlAt(i)->containsTag(text)))
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
    actNewDatabase->setShortcut(QKeySequence("Ctrl+N"));
    actNewDatabase->setStatusTip(tr("Create new database"));
    connect(actNewDatabase, SIGNAL(triggered()), this, SLOT(createDatabase()));

    actOpenUrl = new QAction(QIcon(":/images/go-jump.png"),tr("Open URL"), this);
    actOpenUrl->setShortcut(QKeySequence("Ctrl+O"));
    actOpenUrl->setStatusTip(tr("Open link in web-browser."));
    connect(actOpenUrl, SIGNAL(triggered()), this, SLOT(gotoUrl()));

    actOpenUrlWith = new QAction(tr("Open URL with ..."), this);
    actOpenUrlWith->setShortcut(QKeySequence("Alt+O"));
    actOpenUrlWith->setStatusTip(tr("Open link in other web-browser."));
    connect(actOpenUrlWith, SIGNAL(triggered()), this, SLOT(gotoUrl()));

    actAddUrl = new QAction(QIcon(":/images/list-add.png"), tr("Add URL ..."), this);
    actAddUrl->setShortcut(QKeySequence("Ctrl+A"));
    actAddUrl->setStatusTip(tr("Add new URL in list."));
    connect(actAddUrl, SIGNAL(triggered()), this, SLOT(execAddUrl()));

    actToolAddUrl= new QAction(QIcon(":/images/toolbar/list-add.png"), tr("Add URL ..."), this);
    actToolAddUrl->setStatusTip(tr("Add new URL in list."));
    connect(actToolAddUrl, SIGNAL(triggered()), this, SLOT(execAddUrl()));

    actToolDelUrl= new QAction(QIcon(":/images/toolbar/list-remove.png"), tr("Delete URL ..."), this);
    actToolDelUrl->setStatusTip(tr("Delete URL from list."));
    connect(actToolDelUrl, SIGNAL(triggered()), this, SLOT(delUrl()));

    actToolGoToUrl = new QAction(QIcon(":/images/toolbar/go-jump.png"), tr("Open URL with default web-browser. (Ctrl+O)"), this);
    actToolGoToUrl->setStatusTip(tr("Open URL in with default web-browser."));
    connect(actToolGoToUrl, SIGNAL(triggered()), this, SLOT(gotoUrl()));

    actToolFavorite = new QAction(QIcon(":/images/toolbar/emblem-favorite.png"), tr("Show only favorite links. (Alt+F)"), this);
    actToolFavorite->setShortcut(QKeySequence("Alt+F"));
    actToolFavorite->setStatusTip(tr("Show only favorite links from base."));
    actToolFavorite->setCheckable(true);
    connect(actToolFavorite, SIGNAL(triggered()), this, SLOT(showFavorites()));

    actToolEditUrl = new QAction(QIcon(":/images/toolbar/accessories-text-editor.png"), tr("Edit data about URL ..."), this);
    actToolEditUrl->setStatusTip(tr("Edit data about URL."));
    connect(actToolEditUrl, SIGNAL(triggered()), this, SLOT(execAddUrl()));

    actEditUrl = new QAction(QIcon(":/images/accessories-text-editor.png"), tr("Edit URL ..."), this);
    actEditUrl->setShortcut(QKeySequence("Ctrl+E"));
    actEditUrl->setStatusTip(tr("Edit data about URL."));
    connect(actEditUrl, SIGNAL(triggered()), this, SLOT(execAddUrl()));

    actDelUrl = new QAction(QIcon(":/images/list-remove.png"), tr("Delete URL"), this);
    actDelUrl->setShortcut(QKeySequence("Ctrl+D"));
    actDelUrl->setStatusTip(tr("Delete URL from list."));
    connect(actDelUrl, SIGNAL(triggered()), this , SLOT(delUrl()));

    actSearchUrl = new QAction(this); // при нажатии на Ctrl+F активирует строку ввода для поиска ссылок
    actSearchUrl->setShortcut(QKeySequence("Ctrl+F"));
    connect(actSearchUrl, SIGNAL(triggered()), this, SLOT(setSearchFocus()));

    actExit = new QAction(QIcon(":/images/application-exit.png"), tr("Exit"), this);
    actExit->setShortcut(QKeySequence("Alt+X"));
    actExit->setStatusTip(tr("Close program."));
    connect(actExit, SIGNAL(triggered()), SLOT(exitApp()));

    actOptions = new QAction(tr("Options ..."), this);
    actOptions->setShortcut(QKeySequence("Ctrl+P"));
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
        bool ok;
        autosaveInterval = inText.readLine(0).toInt(&ok, 10);
        restartAutosaveTimer();
        loadDB();
    } else {
        Options(); // если конфига нет, то открываем диалог настроек программы
    }
}

void MainWindow::saveSettings() // сохраняем настройки программы в конфиг
{
    QFile textFile(homeDir + PROGRAM_CONFIG);
    textFile.open(QIODevice::WriteOnly| QIODevice::Text | QIODevice::Truncate);
    QTextStream outText(&textFile);
    outText.setCodec("UFT-8");
    outText << strDefBrowser << endl;
    outText << strPathToDB << endl;
    outText << boolMonitoringClipboard << endl;
    outText << autosaveInterval << endl;
    dataEdited = false;
}

void MainWindow::restartAutosaveTimer()
{
    if (autosaveInterval > 0) {
        if (autosaveTimer == nullptr) {
            autosaveTimer = new QTimer(this);
            connect(autosaveTimer, SIGNAL(timeout()), SLOT(autosaveDB()));
        }
    } else {
        if (autosaveTimer != nullptr) {
            qDebug() << "Stop autosave timer";
            autosaveTimer->stop();
            delete autosaveTimer;
            autosaveTimer = nullptr;
        }
    }
    if (autosaveTimer != nullptr) {
        qDebug() << "Restart timer with interval" << autosaveInterval << "minutes";
        autosaveTimer->stop();
        autosaveTimer->start(autosaveInterval * 60000);
    }
}

void MainWindow::Options() // открываем диалог настройек
{
    OptionsDialog *optionsDialog = new OptionsDialog(this,
                                                     strDefBrowser,
                                                     strPathToDB,
                                                     boolMonitoringClipboard,
                                                     autosaveInterval);
    if (optionsDialog->exec() == QDialog::Accepted)
    {
        strDefBrowser = optionsDialog->defaultBrowser();
        boolMonitoringClipboard = optionsDialog->monitoringClipboard();
        if (strPathToDB.compare(optionsDialog->pathToDb()) != 0) {
            strPathToDB = optionsDialog->pathToDb();
            loadDB();
        }
        if (autosaveInterval != optionsDialog->autosaveInterval()) {
            autosaveInterval = optionsDialog->autosaveInterval();
            restartAutosaveTimer();
        }
        QDir dir(homeDir);
        if (dir.exists(homeDir + PROGRAM_DIR)) {
            saveSettings();
        } else {
            dir.mkdir(homeDir + PROGRAM_DIR);
            saveSettings();
        }
    }
    delete optionsDialog;
}

void MainWindow::getInfo(QListWidgetItem *item) // получаем информацию о выделенной ссылке
{
    urlInfo->setPlainText(linkStructure->urlAt(urlListWidget->row(item))->info());
    labelTags->setText(linkStructure->tagsAt(urlListWidget->row(item)));
}

void MainWindow::selectByTag(QTreeWidgetItem *treeItem, int column)
{
    if (strPathToDB.isEmpty())
        return;
    int count = linkStructure->urlsCount();
    bool isAllTags = treeItem->text(0) == tr("All tags");
    for (int i = 0; i < count; i++)
    {
        QListWidgetItem *item = urlListWidget->item(i);
        if (isAllTags) {
            item->setHidden(false);
        } else {
            QString tag = treeItem->text(0);
            if (linkStructure->urlAt(i)->containsTag(tag)) {
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
            linkStructure->removeUrlAt(curRow);
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
    linkStructure->clearUrlList();
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
    setWindowTitle(PROGRAM_NAME + " [" + QFileInfo(strPathToDB).fileName() +
                   "] - elements in DB " + temp.setNum(linkStructure->urlsCount(), 10) );
}

void MainWindow::changeEvent(QEvent *e)
{
    switch (e->type())
    {
        case QEvent::WindowStateChange:
        {
            if (this->windowState() & Qt::WindowMinimized)
            {
                hide();
            }
            break;
        }
        default:
            break;
    }
    QMainWindow::changeEvent(e);
}

void MainWindow::showFavorites() // скрываем/показываем все избранные ссылки
{
    if (strPathToDB.isEmpty())
        return;
    int count = linkStructure->urlsCount();
    for (int i = 0; i < count; i++) // пройдемся посписку
    {
        if (linkStructure->isFavoriteLink(i) == false) // если эл-т не фаворит
        {
            QListWidgetItem *item = urlListWidget->item(i); // получаем нужный элемент (и его свойства)
            if (actToolFavorite->isChecked() == true) // и если кнопка тулбара нажата
                item->setHidden(true); // скрываем его
            else // если кнопка тулбара НЕ нажата
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
                strTags = temp.remove(0, 5);
                QStringList spTags = strTags.split(',', QString::SkipEmptyParts);
                QStringList::iterator it = spTags.begin();
                for (; it != spTags.end(); ++it) {
                    QString name = *it;
                    QString *aTag = new QString(name.remove(0, 1));
                    tempTags->append(aTag);
                    if (!linkStructure->mainListContainsTag(*aTag)) {
                        linkStructure->appendTagToMainList(aTag);
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
                weburl *url = linkStructure->addUrl(favorite, strLink, strInfo);
                QList<QString*>::iterator it = tempTags->begin();
                for (; it != tempTags->end(); ++it) {
                    linkStructure->addTagToUrl(url, *it);
                }
                tempTags->clear();
                addUrlItem(url);	// добавляем ссылку
            }
        } // end while
        tempTags->clear();
        delete tempTags;
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
    linkStructure->saveDB(strPathToDB);
}

void MainWindow::addUrlItem(weburl *url)
{
    addWidgetItem(url->isFavorite(), url->link());
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
    linkStructure->clearAllTags();
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

void MainWindow::addNewUrlDialog() // добавляем ссылку в список
{
    if (!strPathToDB.isEmpty()) {
        resetList(); // вернемся к исходному списку
        AddUrl *addUrlDialog = new AddUrl(this, -1, linkStructure);
        if (addUrlDialog->exec() == QDialog::Accepted)
        {
            weburl *url = addUrlDialog->getUrl();
            addUrlItem(url);
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

void MainWindow::editUrlDialog() // обновляем измененный элемент списка
{
    if (urlListWidget->count() != 0) {
        AddUrl *addUrlDialog = new AddUrl(this, urlListWidget->currentRow(), linkStructure);
        if (addUrlDialog->exec() == QDialog::Accepted)
        {
            weburl *url = addUrlDialog->getUrl();
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
        addNewUrlDialog();
    if (this->sender() == actEditUrl || this->sender() == actToolEditUrl) // если диалог вызвается из меню "Редактировать адрес"
        editUrlDialog();
}

void MainWindow::createToolBar()
{
    mainToolBar = addToolBar("Main");
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
    QWidget *widget = new QWidget();
    QVBoxLayout *vboxLayout = new QVBoxLayout();
    urlInfo = new QTextEdit(plainTextDoc);
    labelTags = new QLabel(plainTextDoc);
    labelTags->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    vboxLayout->addWidget(urlInfo);
    vboxLayout->addWidget(labelTags);
    vboxLayout->setContentsMargins(4, 4, 4, 4);
    widget->setLayout(vboxLayout);

    plainTextDoc->setWidget(widget);
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

void MainWindow::createTrayMenu()
{
    QMenu *trayMenu = new QMenu(this);

    QAction* action = new QAction(tr("Open Url Collector"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(openApp()));
    trayMenu->addAction(action);

    action = new QAction(tr("Quit"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(quitApp()));
    trayMenu->addAction(action);

    systemTrayIcon->setContextMenu(trayMenu);
}

void MainWindow::About()
{
    QMessageBox::about(this, "About " + PROGRAM_NAME,
    "<font size=14>About " + PROGRAM_NAME +"</font>"
    "<p>program for collect web-links</p>"
    "<br>Created by TechnoMag (C) 2009");
}
