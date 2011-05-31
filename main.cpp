#include <QtGui/QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("yle-downloader-gui");
    MainWindow w;
    w.show();

    return a.exec();
}
