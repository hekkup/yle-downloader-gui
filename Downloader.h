#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QtCore>
#include "ProgressParser.h"

class Downloader: public QObject
{
    Q_OBJECT
public:
    Downloader(QUrl url, QDir destDir, QObject* parent = 0);
    virtual ~Downloader();

    void start();

    bool isStarted() { return m_process != 0; }

    void cancel();

signals:
    void downloadStarted();

    void downloadFileCreated(QString fileName);
    void downloadProgress(int progressPercentage);
    void downloadUnknownProgress(double secondsDownloaded);
    void downloaderOutputWritten(QString line);

    void downloadSucceeded();
    void downloadFailed();

private slots:
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void moreInputAvailable();

private:
    QUrl m_url;
    QDir m_destDir;
    QProcess* m_process;
    ProgressParser m_progressParser;
};

#endif // DOWNLOADER_H
