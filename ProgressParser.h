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
    int m_hashesSeen;
};

#endif // PROGRESSPARSER_H
