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
    void reportUnknownProgress();
    void downloaderOutputWritten(QString line);
    void downloadSucceeded();
    void downloadFailed();

private:
    Ui::MainWindow *ui;

    QSettings m_settings;
    QDir m_destDir;
    Downloader* m_downloader;

    void downloadEnded();
    void setDownloadWidgetsDisabled(bool disabled);

    void updateDestDirLabel();
};

#endif // MAINWINDOW_H
