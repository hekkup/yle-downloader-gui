#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QtCore>
#include "ProgressParser.h"

class Downloader: public QObject
{
    Q_OBJECT
public:
    Downloader(QUrl url, QDir destDir, QString ylePassi, QObject* parent = 0);
    virtual ~Downloader();

    bool isStarted() { return m_process != 0; }

public slots:
    void start();
    void cancel();

signals:
    void downloadStarted();

    void downloadFileCreated(QString fileName);
    void downloadProgress(int progressPercentage);
    void downloadUnknownProgress(double secondsDownloaded);
    void downloaderOutputWritten(QString line);

    void downloadSucceeded();
    void downloadCanceled();
    void downloadFailed();
    void downloadNeedsYlePassi();

private slots:
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void moreInputAvailable();

private:
    QUrl m_url;
    QDir m_destDir;
    QString m_ylePassiCookie;
    QProcess* m_process;
    ProgressParser m_progressParser;
    bool m_cancelRequested;

};

#endif // DOWNLOADER_H
