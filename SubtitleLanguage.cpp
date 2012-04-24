#include "SubtitleLanguage.h"

SubtitleLanguage::SubtitleLanguage(QString displayName, QString optionId) :
    displayName(displayName),
    optionId(optionId)
{
}

QList<SubtitleLanguage> SubtitleLanguage::getAll()
{
    QList<SubtitleLanguage> list;
    list.append(SubtitleLanguage(QObject::tr("None"), "none"));
    list.append(SubtitleLanguage(QObject::tr("Finnish (all text)"), "fih"));
    list.append(SubtitleLanguage(QObject::tr("Finnish (translation)"), "fin"));
    list.append(SubtitleLanguage(QObject::tr("Swedish"), "swe"));
    list.append(SubtitleLanguage(QObject::tr("Saami"), "smi"));
    list.append(SubtitleLanguage(QObject::tr("All"), "all"));
    return list;
}

SubtitleLanguage SubtitleLanguage::getDefault()
{
    return getAll().at(2);
}
