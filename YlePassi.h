#ifndef YLEPASSI_H
#define YLEPASSI_H

#include <QObject>
#include <QString>

class QNetworkAccessManager;

class YlePassi: public QObject
{
    Q_OBJECT
public:
    explicit YlePassi(QString username, QString password, QObject* parent = 0);

    void startLoggingIn();

signals:
    void loginSuccessful(QString cookie);
    void loginFailed();

private slots:
    void loginRequestComplete();

private:
    QString m_username;
    QString m_password;
    QNetworkAccessManager* m_nam;
};

#endif // YLEPASSI_H
