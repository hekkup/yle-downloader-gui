#include <QtGui>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("yle-downloader-gui");

    QTranslator translator;
    translator.load(":/fi");
    app.installTranslator(&translator);

    MainWindow w;
    w.show();

    return app.exec();
}
