#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui>

class Downloader;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void chooseDestDir();

    void startDownload();
    void reportDestFileName(QString name);
    void reportProgress(int percentage);
    void reportUnknownProgress(double secondsDownloaded);
    void downloaderOutputWritten(QString line);
    void downloadSucceeded();
    void downloadCanceled();
    void downloadFailed();

    void cancelRequested();

private:
    Ui::MainWindow *ui;

    QSettings m_settings;
    QDir m_destDir;
    Downloader* m_downloader;
    QString destFileName;

    void downloadEnded(bool success);
    void setDownloadWidgetsDisabled(bool disabled);

    void updateDestDirLabel();

    QString formatSecondsDownloaded(double secondsDownloaded);
};

#endif // MAINWINDOW_H
