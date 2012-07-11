#include "SubtitleLanguage.h"

SubtitleLanguage::SubtitleLanguage(QString displayName, QString optionId) :
    m_displayName(displayName),
    m_optionId(optionId)
{
}

QList<SubtitleLanguage> SubtitleLanguage::getAll()
{
    QList<SubtitleLanguage> list;
    list.append(SubtitleLanguage(QObject::tr("None"), "none"));
    list.append(SubtitleLanguage(QObject::tr("Finnish"), "fin"));
    list.append(SubtitleLanguage(QObject::tr("Finnish (hearing impaired)"), "fih"));
    list.append(SubtitleLanguage(QObject::tr("Swedish"), "swe"));
    list.append(SubtitleLanguage(QObject::tr("Swedish (hearing impaired)"), "swh"));
    list.append(SubtitleLanguage(QObject::tr("Saami"), "smi"));
    list.append(SubtitleLanguage(QObject::tr("English"), "eng"));
    list.append(SubtitleLanguage(QObject::tr("All"), "all"));
    return list;
}

SubtitleLanguage SubtitleLanguage::getDefault()
{
    return getAll().at(2);
}
