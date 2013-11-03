#include "ProgressParser.h"

ProgressParser::ProgressParser(QObject* parent)
    : QObject(parent),
    m_percentage(0),
    m_gotFileName(false),
    m_firstPercentageReported(false)
{
}

ProgressParser::~ProgressParser()
{
}

void ProgressParser::addData(QByteArray buf)
{
    m_lineBuffer.append(buf);
    processBufferedLines();
}

void ProgressParser::processBufferedLines()
{
#ifdef Q_OS_WIN
    // On windows, avoid double-newlines when we replace '\r'->'\n' next.
    // If the buffer happens to end between and '\r' and an '\n',
    // we get an extra newline, which only makes the output slightly uglier.
    m_lineBuffer.replace("\r\n", "\n");
#endif
    m_lineBuffer.replace('\r', '\n'); // Progress lines end in a carriage return only
    QList<QByteArray> lines = m_lineBuffer.split('\n');
    m_lineBuffer = lines.last();
    lines.removeLast();
    foreach (QByteArray line, lines) {
        processLine(QString::fromUtf8(line)); // yle-dl prints utf-8 when on a pipe also on Windows
    }
}

void ProgressParser::processLine(QString line)
{
    line = line.trimmed();
    emit outputLineSeen(line);
    bool done = false;
    done = done || (!m_gotFileName && tryAsFileNameLineLine(line));
    done = done || tryAsProgressLine(line);
    done = done || tryAsUnknownProgressLine(line);
    (void)done;
}

bool ProgressParser::tryAsFileNameLineLine(QString line)
{
    QRegExp regex("^Output file: (.*)$");
    if (regex.exactMatch(line)) {
        QString fileName = regex.cap(1);
        emit fileNameDetermined(fileName);
        return true;
    }
    return false;
}

bool ProgressParser::tryAsProgressLine(QString line)
{
    QRegExp regex("^.* / .* sec \\((\\d+).*%\\)$");
    if (regex.exactMatch(line)) {
        int newPercentage = regex.cap(1).toInt();
        if ((newPercentage > m_percentage) || !m_firstPercentageReported) {
            emit progressMade(newPercentage);
            m_firstPercentageReported = true;
        }
        m_percentage = newPercentage;
        return true;
    }
    return false;
}

bool ProgressParser::tryAsUnknownProgressLine(QString line)
{
    QRegExp regex("^.* / (.*) sec$");
    if (regex.exactMatch(line)) {
        double seconds = regex.cap(1).toDouble();
        emit indeterminateProgressMade(seconds);
        return true;
    }
    return false;
}
