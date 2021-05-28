#include "mainwindow.h"

const QString PROGRAM_NAME="URLCollector v1.4";
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
	
	urlList = new QListWidget(this);
	urlList->setSpacing(1);
	urlList->setSelectionRectVisible(true);
	urlList->setSelectionMode(QAbstractItemView::SingleSelection);
	connect(urlList, SIGNAL(itemClicked(QListWidgetItem *)), this , SLOT(getInfo(QListWidgetItem *)));
	connect(urlList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this , SLOT(gotoUrl()));
	setCentralWidget(urlList);
	
	createActions();
	createMenu();
	createToolBar();
	createStatusBar();
	createDocWindows();
	QTimer::singleShot(0, this, SLOT(initApp()));
    initMonitoringClipboard();
}

void MainWindow::initApp()
{
    readSettings();
}

void MainWindow::createDatabase()
{
    QString pathToNewDB = QFileDialog::getSaveFileName(this, tr("Select file of BataBase"), "/home", "Data Base Of URL's (*.ucl)");
    if (!pathToNewDB.isEmpty()) {
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
        _addItem(false, new weburl(false, board->text(), tr("Added automatically")), false);
        dataEdited = true;
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
        searchClassUrl.clear();
        while(urlList->count() != 0)
            delete urlList->takeItem(0);
        int count = classUrl.size() - 1;
        for (int i = 0; i <= count; i++) {
            addWidgetItem(classUrl[i]->isFavorite(), classUrl[i]->link());
        }
        isSearching = false;
    }
}

void MainWindow::gotoUrl() // открываем браузер с ссылкой
{
    if (urlList->count() != 0 || urlList->currentRow() != -1)
	{
		QStringList args;
        if (searchClassUrl.size() == 0) {
            args << classUrl[urlList->currentRow()]->link();
        } else {
            args << searchClassUrl[urlList->currentRow()]->link();
        }
        if (!strDefBrowser.isEmpty() && (
                this->sender() == actOpenUrl ||
                this->sender() == actToolGoToUrl ||
                this->sender() == urlList)) // если хотим открыть в браузере по-умолчанию
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
    searchClassUrl.clear();
    while(urlList->count() != 0)
        delete urlList->takeItem(0);
    int j = 0;
    int count = classUrl.size();
    do {
        if (!text.isEmpty() && (
            classUrl[j]->link().indexOf(text, 0, Qt::CaseInsensitive) != -1 ||
            classUrl[j]->info().indexOf(text, 0, Qt::CaseInsensitive) != -1))
        {
            _addItem(classUrl[j]->isFavorite(), classUrl[j], true);
        }
        j++;
    } while (count != j);
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
	actOptions->setShortcut(tr("Alt+O"));
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
        if (strMonitoringClipboard == "1")
            boolMonitoringClipboard = true;
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
    OptionsDialog dialog(this, strDefBrowser, strPathToDB, boolMonitoringClipboard);
    if (dialog.exec() == QDialog::Accepted)
	{
        strDefBrowser = dialog.defaultBrowser();
        strPathToDB = dialog.pathToDb();
        boolMonitoringClipboard = dialog.monitoringClipboard();
		QDir dir(homeDir);
        if (dir.exists(homeDir + PROGRAM_DIR)) {
			saveSettings();
        } else {
            dir.mkdir(homeDir + PROGRAM_DIR);
			saveSettings();
		}
        loadDB();
	}
}

void MainWindow::getInfo(QListWidgetItem *item) // получаем информацию о выделенной ссылке
{
    if (isSearching == false) {
        urlInfo->setPlainText(classUrl[urlList->row(item)]->info());
    } else {
        urlInfo->setPlainText(searchClassUrl[urlList->row(item)]->info());
    }
}

void MainWindow::delUrl() // удаляем выделенную ссылку
{
    if (urlList->currentRow() != -1 || urlList->count() != 0)
	{	
        int ret = QMessageBox::question(this,
                                        tr("Delete URL"),
                                        tr("Do you want to delete this web-link?"),
                                        QMessageBox::Yes | QMessageBox::No);
		if (ret == QMessageBox::Yes)
		{
			QString temp;
			int curRow = urlList->currentRow();
			QListWidgetItem *curItem = urlList->takeItem(curRow); // удаляем элемент из QListWidget
			urlList->removeItemWidget(curItem); // удаляем элемент из QListWidgetItem
            weburl *url = nullptr;
            if (isSearching == false) {
                url = classUrl.takeAt(curRow);
            } else {
                url = searchClassUrl.takeAt(curRow);
            }
            classUrl.removeAll(url);
            searchClassUrl.removeAll(url);
            delete url;
            setWindowTitle(PROGRAM_NAME + " - elements in DB " + temp.setNum(classUrl.count(), 10) );
            dataEdited = true;
		}
    } else {
        QMessageBox::warning(this,
                             tr("Delete URL"),
                             tr("No selected items"),
                             QMessageBox::Ok);
    }
}

void MainWindow::clearListItems()
{
    while(urlList->count() != 0)
		delete urlList->takeItem(0);
    classUrl.clear();
}


void MainWindow::showFavorites() // скрываем/показываем все избранные ссылки
{
		QListWidgetItem *item=new QListWidgetItem;
        for (int i = 0; i < classUrl.size(); i++) // пройдемся посписку
		{
            if (classUrl[i]->isFavorite() == false) // если эл-т не фаворит
			{
                item = urlList->item(i); // получаем нужный элемент (и его свойства)
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
        clearListItems();
		// временные строки для хранения инфы
		QString temp;
		QString strLink;
		QString strInfo;
        bool favorite = false; // инициализируем эту ф-ню
		
        qDebug() << "Loading Database ...";
		QTextStream inDB(&textDB);
		inDB.setCodec("UTF-8");
        while (inDB.atEnd() != true) // пока не дочитали до конца файла
		{
			temp=inDB.readLine(0); // читаем строку
            if (temp.startsWith("link: ")) // если встретилась ссылка
                strLink=temp.remove(0, 6); // сохраняем ссылку

            if (temp.startsWith("status: normal")) // если встретился статус
                favorite = false;

            if (temp.startsWith("status: favorite")) // если встретился статус
                favorite = true;

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
                _addItem(favorite, new weburl(favorite, strLink, strInfo), false);	// добавляем ссылку
            }
		} // end while
        textDB.close();
        qDebug() << "Database is loaded.";
		setWindowTitle(PROGRAM_NAME + " - elements in DB " + temp.setNum(classUrl.count(), 10) );
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
        for (int i = 0; i < classUrl.size(); i++) // пройдемся поо БД
		{
			outDB << "link: " << classUrl[i]->link() << endl;
            if (classUrl[i]->isFavorite() == true)
				outDB << "status: favorite" << endl;
			else
				outDB << "status: normal" << endl;
			
			outDB << "info: " << endl << classUrl[i]->info() << endl << "endinfo." << endl;
		}
        textDB.close();
	}
}

void MainWindow::_addItem(bool favorite, weburl *url, bool isSearching)
// добавление эл-та в базу
// bool gui - длбавляем и в список и в память?
{
    addWidgetItem(favorite, url->link());
    if (isSearching == false) { // если добавляем элемент в базу
        classUrl.append(url);
    } else { // добавляем найденный элемент
        searchClassUrl.append(url);
    }
}

void MainWindow::setItemFavorite(bool favorite, QListWidgetItem *newItem) // настрока вида элемента
{
    if (favorite == true)
    {
        newItem->setBackground(QBrush(QColor(249, 251, 205), Qt::SolidPattern));
        newItem->setIcon(QIcon(":/images/bookmark-new.png"));
    }
    else
    {
        newItem->setIcon(QIcon(":/images/text-html.png"));
        newItem->setBackground(QBrush(QColor(205, 240, 251), Qt::SolidPattern));
    }
}

void MainWindow::initMonitoringClipboard()
{
    board = QGuiApplication::clipboard();
    connect(board, &QClipboard::dataChanged, this, &MainWindow::clipboardChanged, Qt::DirectConnection);
}

void MainWindow::addWidgetItem(bool favorite, QString text) // добавляем виджет эелмента в список
{
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(text);
    setItemFavorite(favorite, newItem);
    urlList->addItem(newItem);
}

void MainWindow::addItemToList() // добавляем ссылку в список
{
    if (!strPathToDB.isEmpty()) {
        resetList(); // вернемся к исходному списку
        AddUrl dialog(this, false, nullptr);
        if (dialog.exec() == QDialog::Accepted)
        {
            QString temp;
            weburl *url = new weburl(dialog.isFavorite(), dialog.webUrl(), dialog.infoUrl());
            _addItem(dialog.isFavorite(), url, false);
            setWindowTitle( PROGRAM_NAME + " - elements in DB " + temp.setNum(classUrl.count(), 10) );
            dataEdited = true;
        }
    } else {
        QMessageBox::critical(this,
                              tr("Edit database"),
                              tr("Database is not opened!"),
                              QMessageBox::Ok);
    }
}

void MainWindow::refreshItem() // обновляем измененный элемент списка
{
    if (urlList->count() != 0) {
        weburl *url = nullptr;
        if (isSearching == false) {
            url = classUrl[urlList->currentRow()];
        } else {
            url = searchClassUrl[urlList->currentRow()];
        }
        AddUrl dialog(this, true, url);
        if (dialog.exec() == QDialog::Accepted)
		{
            url->setLink(dialog.webUrl());
            url->setInfo(dialog.infoUrl());
            url->setFavorite(dialog.isFavorite());
			QListWidgetItem *curItem = urlList->currentItem();
            curItem->setText(dialog.webUrl());
            setItemFavorite(url->isFavorite(), curItem);
            urlInfo->setPlainText(url->info());
            dataEdited = true;
		}
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
}

void MainWindow::About()
{
	QMessageBox::about(this, "About " + PROGRAM_NAME,
	"<font size=14>About " + PROGRAM_NAME +"</font>"
	"<p>program for collect web-links</p>"
	"<br>Created by TechnoMag (C) 2009");
}
