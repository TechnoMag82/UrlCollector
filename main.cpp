#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);
    QTranslator myTranslator;
    QString locale = QLocale::system().name();
    myTranslator.load(QString(":/translations/main_" + locale.left(2)));
    app.installTranslator(&myTranslator);
    MainWindow *mainWin = new MainWindow;
    mainWin->show();
    return app.exec();
}
