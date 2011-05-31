#include "ProgressParser.h"

ProgressParser::ProgressParser()
    : m_hashesSeen(0)
{
}

void ProgressParser::addData(QByteArray buf)
{
    // This does not work for live streams with no known length.
    // Also, it may be inaccurate if the preliminary output happens to contain hashes.
    // FIXME: read status messages instead of hashes
    foreach (char ch, buf) {
        if (ch == '#') {
            m_hashesSeen += 1;
        }
    }
}

int ProgressParser::getProgressPercentage()
{
    return m_hashesSeen * 10;
}
