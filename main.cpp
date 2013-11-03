#ifdef QT_5_0
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "MainWindow.h"
#include "config.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

namespace
{

bool looksLikeUrl(QString s)
{
    return s.startsWith("http://");
}

void setApplicationLanguage(QApplication* app, QLocale language)
{
    if (!app) {
        return;
    }
    QTranslator* builtInTranslator = new QTranslator();
    builtInTranslator->load("qt_" + language.name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app->installTranslator(builtInTranslator);

    QTranslator* translator = new QTranslator();
    translator->load(":/" + language.name());
    app->installTranslator(translator);
}

}

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString toolDir = QDir::current().absoluteFilePath(WINDOWS_YLE_DL_DIR).replace('/', '\\');
    QString newPath = toolDir + ";" + env.value("PATH");
    ::SetEnvironmentVariableA("PATH", newPath.toLocal8Bit());
#endif

    QApplication app(argc, argv);
    QApplication::setOrganizationName("nobody");
    QApplication::setApplicationName("yle-downloader-gui");

    QSettings settings;
    QVariant localeNameVar = settings.value("language", QVariant());
    QLocale locale;
    if (!localeNameVar.isValid()) {
        locale = QLocale::Finnish;
        settings.setValue("language", locale.name());
    } else {
        locale = QLocale(localeNameVar.toString());
    }

    setApplicationLanguage(&app, locale);

    MainWindow w;
    w.setWindowIcon(QIcon(":/icons/icon.ico"));
    w.setLocale(locale);
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
