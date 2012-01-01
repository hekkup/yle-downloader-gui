#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui>
#include "UpdateChecker.h"
#include "YlePassi.h"

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

private slots:
    void chooseDestDir();

    void startDownload();
    void startDownload(QString ylePassiCookie);
    void reportDestFileName(QString name);
    void reportProgress(int percentage);
    void reportUnknownProgress(double secondsDownloaded);
    void downloaderOutputWritten(QString line);
    void downloadSucceeded();
    void downloadCanceled();
    void downloadFailed();
    void downloadNeedsYlePassi();
    void ylePassiLoginFailed();

    void cancelRequested();

    void updateAvailable(QString version, QUrl url);

    void openUrl(QString url);

protected:
    void closeEvent(QCloseEvent* event);

private:
    Ui::MainWindow *ui;

    QSettings m_settings;
    QDir m_destDir;
    Downloader* m_downloader; // may be null
    QString m_destFileName;
    UpdateChecker* m_updateChecker;
    YlePassi* m_ylePassi; // may be null

    bool confirmCancel();

    void downloadEnded(bool success);
    void setDownloadWidgetsDisabled(bool disabled);

    void updateDestDirLabel();

    QString formatSecondsDownloaded(double secondsDownloaded);
    QNetworkAccessManager *m_networkManager;
    static QDir defaultDestDir();
};

#endif // MAINWINDOW_H
