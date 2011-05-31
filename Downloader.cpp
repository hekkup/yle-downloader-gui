
#include "Downloader.h"
#include <cstdio>

Downloader::Downloader(QUrl url, QDir destDir, QObject* parent)
    : QObject(parent), m_url(url), m_destDir(destDir), m_process(0)
{
}

Downloader::~Downloader()
{
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->kill();
    }
}

void Downloader::start()
{
    Q_ASSERT(!isStarted());

    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::MergedChannels);

    connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)));
    connect(m_process, SIGNAL(readyRead()), this, SLOT(moreInputAvailable()));

    m_process->setWorkingDirectory(m_destDir.absolutePath());
    m_process->start("yle-dl", QStringList() << "--hashes" << m_url.toString());

    m_process->waitForStarted(-1);
    emit downloadStarted();
}

void Downloader::cancel()
{
    Q_ASSERT(isStarted());
    m_process->kill();
}

void Downloader::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        emit downloadSucceeded();
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
    emit downloadProgress(m_progressParser.getProgressPercentage());
}
