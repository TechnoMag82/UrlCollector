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
#include <QSystemTrayIcon>
#include <QApplication>
#include <QDesktopWidget>

#include "weburl.h"
#include "addurl.h"
#include "optionsdialog.h"
#include "linkstructure.h"
#include "urlitemdelegate.h"
#include "consts.h"

struct Settings {
    QString strDefBrowser;
    QString strPathToDB;
    bool boolMonitoringClipboard = false;
    int autosaveInterval = 0;
    QRect windowPosition = QRect(-1, -1, -1, -1);
    bool windowVisible = false;
};

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
        void changeEvent(QEvent* e);

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
        void openApp();
        void quitApp();
        void exitApp();
        void autosaveDB();
        void saveAll();

        void copyLinkToClipboard();
        void setFavoriteLink();
        void editLink();
        void deleteLink();

    private:
        QMenu *menuUrl;
            QAction *actNewDatabase;
            QAction *actOpenUrl;
            QAction *actOpenUrlWith;
            QAction *actAddUrl;
            QAction *actEditUrl;
            QAction *actDelUrl;
            QAction *actSaveAll;
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
        QMenu *popupMenuUrlList;
            QAction *actPopupIsFavorite;
        QLabel *labelTags;
        QSystemTrayIcon *systemTrayIcon;

        void createLockFile();
        void removeLockFile();

        void createMenu();
        void createActions();
        void createStatusBar();
        void createToolBar();
        void createDocWindows();
        void createTagsPopupMenu();
        void createUrlListPopupMenu();
        void createTrayMenu();

        bool readSettings();
        void saveSettings();
        void restartAutosaveTimer();

        bool loadDB();
        void saveDB();
        void addNewUrlDialog();
        void addUrlItem(weburl *url);
        void addTagWidgetItem(const QString &tag);
        void addWidgetItem(/*bool favorite, QString text*/weburl *url);
        void editUrlDialog();
        void clearUrlList();

        void selectBrowser(QStringList args);
        void resetList();
//        void setItemFavorite(bool favorite, QListWidgetItem *newItem);
        void initMonitoringClipboard();
        void addRootTreeItem();

        void clearAllTags();
        bool treeContainTag(const QString &tag);
        void updateTags(weburl *url);

        void updateWindowsTitle();

        LinkStructure *linkStructure = nullptr;

        // Options
        Settings settings;

        QTimer *autosaveTimer = nullptr;

        QClipboard *board = nullptr;
        QString oldClipboard;

        QString homeDir;
        bool dataEdited;
        bool isSearching = false;

        QTreeWidgetItem *selectedTagItem;
        QListWidgetItem *selectedUrlItem;
        int selectedTagIndex = -1;
        int selectedUrlIndex = -1;

        QTreeWidgetItem *rootTagsItem = nullptr;
        QFile *lockFile = nullptr;
};

#endif
