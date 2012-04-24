#ifndef SUBTITLELANGUAGE_H
#define SUBTITLELANGUAGE_H

#include <QtCore>
#include <QList>
#include <QLocale>

class SubtitleLanguage
{
public:
    SubtitleLanguage(QString displayName, QString optionId);

    const QString displayName;
    const QString optionId;

    static QList<SubtitleLanguage> getAll();
    static SubtitleLanguage getDefault();
};

#endif // SUBTITLELANGUAGE_H
