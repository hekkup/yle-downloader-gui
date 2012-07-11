#ifndef SUBTITLELANGUAGE_H
#define SUBTITLELANGUAGE_H

#include <QtCore>
#include <QList>
#include <QLocale>

class SubtitleLanguage
{
public:
    SubtitleLanguage(QString displayName, QString optionId);

    QString displayName() const { return m_displayName; }
    QString optionId() const { return m_optionId; }

    static QList<SubtitleLanguage> getAll();
    static SubtitleLanguage getDefault();

private:
    QString m_displayName;
    QString m_optionId;
};

#endif // SUBTITLELANGUAGE_H
