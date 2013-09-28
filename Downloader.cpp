
#include "Downloader.h"
#include "ProcessUtils.h"
#include "config.h"
#include <cstdio>

Downloader::Downloader(QUrl url, QDir destDir, QObject* parent)
    : QObject(parent), m_url(url), m_destDir(destDir), m_process(0), m_cancelRequested(false),
    m_downloading(false), m_downloaderMode(Downloader::ModeDownload)
{
    connect(&m_progressParser, SIGNAL(fileNameDetermined(QString)), this, SIGNAL(downloadFileCreated(QString)));
    connect(&m_progressParser, SIGNAL(progressMade(int)), this, SIGNAL(downloadProgress(int)));
    connect(&m_progressParser, SIGNAL(indeterminateProgressMade(double)), this, SIGNAL(downloadUnknownProgress(double)));
    connect(&m_progressParser, SIGNAL(outputLineSeen(QString)), this, SIGNAL(downloaderOutputWritten(QString)));
}

Downloader::~Downloader()
{
    if (m_process && m_process->state() != QProcess::NotRunning) {
        ProcessUtils::killProcessTree(*m_process);
        m_process->waitForFinished(5000);
    }
}

void Downloader::start(Downloader::DownloaderMode mode)
{
    if (isStarted()) {
        return;
    }
    if (Downloader::MODE_COUNT <= mode) {
        return;
    }
    m_downloaderMode = mode;

    m_process = new QProcess(this);
    if (Downloader::ModeDownload == m_downloaderMode) {
        // yle-dl outputs log messages (progress, file names) to stderr
        m_process->setProcessChannelMode(QProcess::MergedChannels);
        m_progressParser.setParsingEnabled(true);
    } else {
        // URL's and media titles are printed to stdout (good!)
        m_process->setProcessChannelMode(QProcess::SeparateChannels);
        m_process->setReadChannel(QProcess::StandardOutput);
        m_progressParser.setParsingEnabled(false);
    }

    connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)));
    connect(m_process, SIGNAL(readyRead()), this, SLOT(moreInputAvailable()));

    QString binary = "yle-dl";
    QStringList arguments;

#ifdef Q_WS_WIN
    QDir yleDlDir = QDir::current();
    yleDlDir.cd(WINDOWS_YLE_DL_DIR);

    arguments << "--vfat";

    arguments << "--rtmpdump" << yleDlDir.absoluteFilePath("rtmpdump.exe");
#endif

    m_process->setWorkingDirectory(m_destDir.absolutePath());

    if (!m_subtitles.isEmpty()) {
        arguments << "--sublang" << m_subtitles;
    }

    if (Downloader::ModeDownload == m_downloaderMode) {
        // hopefully m_extraArgs doesn't contain --showurl or --showtitle
        arguments << m_extraArgs;
    }
    else if (Downloader::ModePeekUrl == m_downloaderMode) {
        arguments << "--showurl";
    }
    else if (Downloader::ModePeekTitle == m_downloaderMode) {
        arguments << "--showtitle";
    }

    arguments << m_url.toString();

    m_process->start(binary, arguments);

    if (m_process->waitForStarted(-1)) {
        m_downloading = true;
        emit downloadStarted();
    } else {
        qDebug() << m_process->errorString();
        m_downloading = false;
        emit downloadFailed();
    }
}


void Downloader::cancel()
{
    if (!isStarted()) {
        return;
    }
    m_cancelRequested = true;
    ProcessUtils::killProcessTree(*m_process);
}

void Downloader::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_downloading = false;
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        emit downloadSucceeded();
    } else if (m_cancelRequested) {
        emit downloadCanceled();
    } else {
        emit downloadFailed();
    }
}

void Downloader::moreInputAvailable()
{
    QByteArray buf = m_process->readAll();

    std::fwrite(buf.data(), 1, buf.length(), stdout);
    std::fflush(stdout);

    m_progressParser.addData(buf);
}
