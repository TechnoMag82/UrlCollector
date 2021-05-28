#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
#include <QToolBar>
#include <QLineEdit>
#include <QFileDialog>
#include <QMenuBar>
#include <QStatusBar>
#include <QDockWidget>

#include "weburl.h"
#include "addurl.h"
#include "optionsdialog.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
	public:
		MainWindow();
	protected:
		void closeEvent(QCloseEvent *event);
	private slots:
		void About();
		void execAddUrl();
		void getInfo(QListWidgetItem *item);
		void delUrl();
		void gotoUrl();
		void Options();
		void setSearchFocus();
		void showFavorites();
		void searchInDB(const QString text);
		void initApp();
        void createDatabase();
        void clipboardChanged();
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
			
		QListWidget *urlList; // список с ссылками
		QTextEdit *urlInfo; // в нем показывается инфа о ссылке
		QLineEdit *search; // строка ввода поиска
		
		void createMenu();
		void createActions();
		void createStatusBar();
		void createToolBar();
		void createDocWindows();
		
		void readSettings();
		void saveSettings();
		
		bool loadDB();
		void saveDB();
		void addItemToList();
        void _addItem(bool favorite, weburl *url, bool isSearching);
        void addWidgetItem(bool favorite, QString text);
		void refreshItem();
        void clearListItems();

        void selectBrowser(QStringList args);
        void resetList();
        void setItemFavorite(bool favorite, QListWidgetItem *newItem);
        void initMonitoringClipboard();

        QList <weburl* > classUrl;
        QList <weburl* > searchClassUrl;

        // Options
        QString strDefBrowser;
        QString strPathToDB;
        bool boolMonitoringClipboard = false;

        QClipboard *board = nullptr;
        QString oldClipboard;

		QString homeDir;
		bool dataEdited;
        bool isSearching = false;
};

#endif
