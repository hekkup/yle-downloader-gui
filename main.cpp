#include <QtGui>
#include "MainWindow.h"
#include "config.h"

#ifdef Q_WS_WIN
#include <Windows.h>
#endif

namespace
{

bool looksLikeUrl(QString s)
{
    return s.startsWith("http://");
}

}

int main(int argc, char *argv[])
{
#ifdef Q_WS_WIN
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString toolDir = QDir::current().absoluteFilePath(WINDOWS_YLE_DL_DIR).replace('/', '\\');
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
    w.setWindowIcon(QIcon(":/icons/icon.ico"));
    if (QApplication::arguments().size() > 0 && looksLikeUrl(QApplication::arguments().last())) {
        w.setExitOnSuccess(true);
        w.setSaveAndRestoreSession(false);
        w.init();
        w.startDownloadFrom(QApplication::arguments().last());
    } else {
        w.setSaveAndRestoreSession(true);
        w.init();
    }
    w.show();

    return app.exec();
}
