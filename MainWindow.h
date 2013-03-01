#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtWidgets>
#include "UpdateChecker.h"

class Downloader;

namespace Ui {
    class MainWindow;
}

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void startDownloadFrom(QString url);
    void setExitOnSuccess(bool value) { m_exitOnSuccess = value; }

private slots:
    void chooseDestDir();

    void saveSubtitlesChoice();

    void startDownload();
    void reportDestFileName(QString name);
    void reportProgress(int percentage);
    void reportUnknownProgress(double secondsDownloaded);
    void downloaderOutputWritten(QString line);
    void downloadSucceeded();
    void downloadCanceled();
    void downloadFailed();

    void cancelRequested();

    void updateAvailable(QString version, QUrl url);

    void openUrl(QString url);

protected:
    void closeEvent(QCloseEvent* event);

private:
    Ui::MainWindow *ui;

    QSettings m_settings;
    QDir m_destDir;
    QString m_destFileName;
    UpdateChecker* m_updateChecker;
    Downloader* m_downloader; // initially null
    bool m_downloadInProgress;
    bool m_exitOnSuccess;

    void initSubtitlesComboBox();

    bool confirmCancel();

    void downloadEnded(bool success);
    void setDownloadWidgetsDisabled(bool disabled);

    void updateDestDirLabel();

    QString formatSecondsDownloaded(double secondsDownloaded);
    QNetworkAccessManager *m_networkManager;
    static QDir defaultDestDir();
};

#endif // MAINWINDOW_H
