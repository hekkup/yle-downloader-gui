#ifndef PROGRESSPARSER_H
#define PROGRESSPARSER_H

#include <QtCore>

class ProgressParser
{
public:
    ProgressParser();

    void addData(QByteArray buf);
    bool isProgressAsPercentage() { return m_percentageKnown; }
    int getProgress()
    {
        if (m_percentageKnown) {
            return m_percentage;
        } else {
            return m_indeterminateProgress;
        }
    }

private:
    bool m_percentageKnown;
    int m_percentage;
    int m_indeterminateProgress;
    QByteArray m_lineBuffer;

    void processBufferedLines();
    void processLine(QString line);
    bool tryAsProgressLine(QString line);
    bool tryAsUnknownProgressLine(QString line);
};

#endif // PROGRESSPARSER_H
