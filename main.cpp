#include "mainwindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTranslator myTranslator;
    QString locale = QLocale::system().name();
    myTranslator.load(QString(":/translations/main_" + locale.left(2)));
    app.installTranslator(&myTranslator);
	MainWindow *mainWin = new MainWindow;
    QDesktopWidget *widget = app.desktop();
    int height = widget->screenGeometry().height();
    int width = widget->screenGeometry().width();
    int x = (width - 800) / 2.0;
    int y = (height - 600) / 2.0;
    mainWin->setGeometry(x, y, 800, 600);
	mainWin->show();
	return app.exec();
}
