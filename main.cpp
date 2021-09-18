#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>

#include "consts.h"

int main(int argc, char *argv[])
{
    int returnCode;
    if (QFile::exists(QDir::homePath() + LOCK_FILE)) {
        return 0;
    }
    QApplication app(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);
    QTranslator myTranslator;
    QString locale = QLocale::system().name();
    myTranslator.load(QString(":/translations/main_" + locale.left(2)));
    app.installTranslator(&myTranslator);
    MainWindow *mainWin = new MainWindow();
    mainWin->show();
    returnCode = app.exec();
    delete mainWin;
    return returnCode;
}
