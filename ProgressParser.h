#ifndef PROGRESSPARSER_H
#define PROGRESSPARSER_H

#include <QtCore>

class ProgressParser: public QObject
{
    Q_OBJECT
public:
    ProgressParser(QObject* parent = 0);
    virtual ~ProgressParser();

    void addData(QByteArray buf);

signals:
    void fileNameDetermined(QString fileName);
    void progressMade(int percentage);
    void indeterminateProgressMade(double secondsDownloaded);
    void outputLineSeen(QString line);

private:
    int m_percentage;
    bool m_gotFileName;
    QByteArray m_lineBuffer;
    bool m_firstPercentageReported;

    void processBufferedLines();
    void processLine(QString line);
    bool tryAsFileNameLineLine(QString line);
    bool tryAsProgressLine(QString line);
    bool tryAsUnknownProgressLine(QString line);

};

#endif // PROGRESSPARSER_H
