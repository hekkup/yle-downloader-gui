#ifndef PROGRESSPARSER_H
#define PROGRESSPARSER_H

#include <QtCore>

class ProgressParser
{
public:
    ProgressParser();

    void addData(QByteArray buf);
    int getProgressPercentage();

private:
    int m_percentage;
    QByteArray m_lineBuffer;

    void processBufferedLines();
    void processLine(QString line);
};

#endif // PROGRESSPARSER_H
