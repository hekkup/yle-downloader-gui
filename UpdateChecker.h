#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QtCore>
#include <QtNetwork>

class UpdateChecker: public QObject
{
    Q_OBJECT
public:
    explicit UpdateChecker(QObject *parent = 0);

signals:
    void updateAvailable(QString version, QUrl url);

public slots:
    void checkForUpdate();

private slots:
    void replyFinished(QNetworkReply*);

private:
    QNetworkAccessManager* m_nam;

    bool isNewerVersion(QString remoteVersion);
    QString cleanVersionNum(QString version);
};

#endif // UPDATECHECKER_H
