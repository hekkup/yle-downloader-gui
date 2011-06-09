#include <QtGui>
#include "MainWindow.h"

#ifdef Q_WS_WIN
#include <Windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef Q_WS_WIN
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString toolDir = QDir::current().absoluteFilePath("rtmpdump-yle-windows-1.4.2").replace('/', '\\');
    QString newPath = toolDir + ";" + env.value("PATH");
    ::SetEnvironmentVariableA("PATH", newPath.toLocal8Bit());
#endif

    QApplication app(argc, argv);
    QApplication::setOrganizationName("nobody");
    QApplication::setApplicationName("yle-downloader-gui");

    QLocale::setDefault(QLocale::Finnish);

    QTranslator translator;
    translator.load(":/fi");
    app.installTranslator(&translator);

    MainWindow w;
    w.show();

    return app.exec();
}
