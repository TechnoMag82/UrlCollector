#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QList>
#include <QToolButton>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QTimer>
#include <QtGui>
#include <QAction>
#include <QMenu>
#include <QTextEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QToolBar>
#include <QLineEdit>
#include <QFileDialog>
#include <QMenuBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QInputDialog>

#include <stdlib.h>
#include <time.h>

#include "weburl.h"
#include "addurl.h"
#include "optionsdialog.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
	public:
		MainWindow();
        ~MainWindow();
        QTreeWidgetItem *getRoot() const;
        void setRoot(QTreeWidgetItem *value);

protected:
        void closeEvent(QCloseEvent *event);
private slots:
        void About();
		void execAddUrl();
		void getInfo(QListWidgetItem *item);
        void selectByTag(QTreeWidgetItem *treeItem, int column);
		void delUrl();
		void gotoUrl();
		void Options();
		void setSearchFocus();
		void showFavorites();
		void searchInDB(const QString text);
		void initApp();
        void createDatabase();
        void clipboardChanged();
        void renameTag();
        void deleteTag();
        void customMenuRequested(QPoint pos);
	private:
		QMenu *menuUrl;
            QAction *actNewDatabase;
			QAction *actOpenUrl;
			QAction *actOpenUrlWith;
			QAction *actAddUrl;
			QAction *actEditUrl;
			QAction *actDelUrl;
			QAction *actExit;
		
		QMenu *menuProgram;
			QAction *actOptions;
			
		QMenu *menuHelp;
			QAction *actAbout;
		
		QToolBar *mainToolBar;
			QAction *actToolAddUrl;
			QAction *actToolEditUrl;
			QAction *actToolDelUrl;
			QAction *actToolGoToUrl;
			QAction *actSearchUrl;
			QAction *actToolFavorite;
			
        QListWidget *urlListWidget; // список с ссылками
        QTreeWidget *tagListWidget; // список тэгов
		QTextEdit *urlInfo; // в нем показывается инфа о ссылке
		QLineEdit *search; // строка ввода поиска
        QMenu *popupMenuTags;
		
		void createMenu();
		void createActions();
		void createStatusBar();
		void createToolBar();
		void createDocWindows();
        void createTagsPopupMenu();
		
		void readSettings();
		void saveSettings();
		
		bool loadDB();
		void saveDB();
		void addItemToList();
        void _addItem(weburl *url);
        void addTagWidgetItem(const QString &tag);
        void addWidgetItem(bool favorite, QString text);
		void refreshItem();
        void clearUrlList();

        void selectBrowser(QStringList args);
        void resetList();
        void setItemFavorite(bool favorite, QListWidgetItem *newItem);
        void initMonitoringClipboard();
        void addRootTreeItem();

        QColor randomColor();

        void clearTags();
        bool containsTag(const QString &tag);

        QList<weburl*> *listUrl = nullptr;
        QList<QString*> *allTags = nullptr;

        // Options
        QString strDefBrowser;
        QString strPathToDB;
        bool boolMonitoringClipboard = false;

        QClipboard *board = nullptr;
        QString oldClipboard;

		QString homeDir;
		bool dataEdited;
        bool isSearching = false;

        QTreeWidgetItem *selectedTagItem;
        int selectedTagIndex = -1;

        QTreeWidgetItem *rootTagsItem = nullptr;
};

#endif
