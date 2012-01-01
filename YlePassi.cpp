#include "YlePassi.h"
#include <QtNetwork>

YlePassi::YlePassi(QString username, QString password, QObject* parent)
    : QObject(parent), m_username(username), m_password(password), m_nam(0)
{
}

void YlePassi::startLoggingIn()
{
    QNetworkRequest request;
    request.setUrl(QUrl("https://login.yle.fi/login/index.php"));

    QUrl postData;
    postData.addQueryItem("username", m_username);
    postData.addQueryItem("password", m_password);

    if (!m_nam) {
        m_nam = new QNetworkAccessManager(this);
    }
    QNetworkReply *reply = m_nam->post(request, postData.encodedQuery());

    connect(reply, SIGNAL(finished()), this, SLOT(loginRequestComplete()));
}

void YlePassi::loginRequestComplete()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply != 0);

    reply->readAll();
    if (reply->hasRawHeader("Set-Cookie")) {
        QString cookie(reply->rawHeader("Set-Cookie"));
        int from = cookie.indexOf("=") + 1;
        cookie = cookie.mid(from, cookie.indexOf(";") - from);
        emit loginSuccessful(cookie);
    } else {
        emit loginFailed();
    }
    reply->deleteLater();
}
