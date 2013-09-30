#include "UpdateChecker.h"
#include "config.h"
#include <QtXml>

UpdateChecker::UpdateChecker(QObject *parent)
    : QObject(parent),
      m_nam(new QNetworkAccessManager(this))
{
    connect(m_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

void UpdateChecker::checkForUpdate()
{
    QUrl downloadUrl("https://raw.github.com/mpartel/yle-downloader-gui/master/update.xml");
    QNetworkRequest req(downloadUrl);
    m_nam->get(req);
}

void UpdateChecker::replyFinished(QNetworkReply* reply)
{
    QString versionTag =
#ifdef Q_OS_WIN
            "latestWindowsVersion";
#else
            "latestSourceVersion";
#endif

    QDomDocument doc;
    if (doc.setContent(reply)) {
        QDomElement root = doc.firstChildElement();
        QDomElement versionElement = root.firstChildElement(versionTag);
        QString version = versionElement.attribute("version");
        QUrl url = QUrl(versionElement.attribute("url"));

        if (version.isEmpty()) {
            qWarning() << "No version string found in update.xml";
            return;
        }

        if (!url.isValid()) {
            qWarning() << "No download URL found in update.xml";
            return;
        }

        if (isNewerVersion(version)) {
            emit updateAvailable(version, url);
        }
    } else {
        qWarning() << "Failed to parse downloaded update.xml";
    }
}

bool UpdateChecker::isNewerVersion(QString remoteVersion)
{
    QStringList parts = QString(cleanVersionNum(YLE_DOWNLOADER_GUI_VERSION)).split('.');
    QStringList remoteParts = cleanVersionNum(remoteVersion).split('.');
    int partCount = qMax(parts.length(), remoteParts.length());

    for (int i = 0; i < partCount; ++i) {
        if (i == parts.size()) {
            parts.append("0");
        }
        if (i == remoteParts.size()) {
            remoteParts.append("0");
        }

        int local = parts.at(i).toInt();
        int remote = remoteParts.at(i).toInt();
        if (local < remote) {
            return true;
        } else if (local > remote) {
            return false;
        }
    }

    return false;
}

QString UpdateChecker::cleanVersionNum(QString version)
{
    return version.replace(QRegExp("[^0123456789.]"), "");
}
