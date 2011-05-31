#include "ProgressParser.h"

ProgressParser::ProgressParser()
    : m_percentage(0)
{
}

void ProgressParser::addData(QByteArray buf)
{
    m_lineBuffer.append(buf);
    processBufferedLines();
}

int ProgressParser::getProgressPercentage()
{
    return m_percentage;
}

void ProgressParser::processBufferedLines()
{
    m_lineBuffer.replace('\r', '\n');
    QList<QByteArray> lines = m_lineBuffer.split('\n');
    m_lineBuffer = lines.last();
    lines.removeLast();
    foreach (QByteArray line, lines) {
        processLine(QString(line));
    }
}

void ProgressParser::processLine(QString line)
{
    line = line.trimmed();
    QRegExp progressRegex("^.* / .* sec \\((\\d+).*%\\)$");
    if (progressRegex.exactMatch(line)) {
        m_percentage = progressRegex.cap(1).toInt();
    }
}
