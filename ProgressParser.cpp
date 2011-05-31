#include "ProgressParser.h"

ProgressParser::ProgressParser()
    : m_percentageKnown(false),
    m_percentage(0),
    m_indeterminateProgress(0)
{
}

void ProgressParser::addData(QByteArray buf)
{
    m_lineBuffer.append(buf);
    processBufferedLines();
}

void ProgressParser::processBufferedLines()
{
    m_lineBuffer.replace('\r', '\n'); // Progress lines end in a carriage return
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
    if (!tryAsProgressLine(line)) {
        tryAsUnknownProgressLine(line);
    }
}

bool ProgressParser::tryAsProgressLine(QString line)
{
    QRegExp regex("^.* / .* sec \\((\\d+).*%\\)$");
    if (regex.exactMatch(line)) {
        m_percentageKnown = true;
        m_percentage = regex.cap(1).toInt();
        return true;
    }
    return false;
}

bool ProgressParser::tryAsUnknownProgressLine(QString line)
{
    QRegExp regex("^.* / .* sec$");
    if (regex.exactMatch(line)) {
        m_percentageKnown = false;
        m_indeterminateProgress += 1;
        return true;
    }
    return false;
}
