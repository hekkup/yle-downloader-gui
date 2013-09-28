#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QtCore>
#include "ProgressParser.h"

class Downloader: public QObject
{
    Q_OBJECT
public:
    enum DownloaderMode {
        ModeDownload = 0,   ///< normal download
        ModePeekUrl,        ///< get download URL(s)
        ModePeekTitle,      ///< get titles of media clips
        MODE_COUNT
    };

    Downloader(QUrl url, QDir destDir, QObject* parent = 0);
    virtual ~Downloader();

    bool isStarted() { return m_process != 0; }

    QString subtitles() const { return m_subtitles; }
    void setSubtitles(QString subtitles) { m_subtitles = subtitles; }

    QStringList extraArgs() const { return m_extraArgs; }
    void setExtraArgs(QStringList extraArgs) { m_extraArgs = extraArgs; }

    Downloader::DownloaderMode mode() { return m_downloaderMode; }

public slots:
    void start(Downloader::DownloaderMode mode = Downloader::ModeDownload);
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

private slots:
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void moreInputAvailable();

private:
    QUrl m_url;
    QDir m_destDir;
    QString m_subtitles;
    QStringList m_extraArgs;
    QProcess* m_process;
    ProgressParser m_progressParser;

    bool m_downloading;
    bool m_cancelRequested;

    Downloader::DownloaderMode m_downloaderMode;
};

#endif // DOWNLOADER_H
