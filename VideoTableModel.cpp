#include "VideoTableModel.h"

VideoTableModel::VideoTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    m_tableHeaderStrings[VideoTableModel::UrlColumn] = tr("URL");
    m_tableHeaderStrings[VideoTableModel::ProgressColumn] = tr("Progress");
    m_tableHeaderStrings[VideoTableModel::StatusColumn] = tr("Status");
    m_addUrlString = tr("<add URL>");

    connect(this, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this,
            SLOT(cleanUpEmptyLines(QModelIndex, QModelIndex)));
}

VideoTableModel::~VideoTableModel() {
    while (m_videos.size()) {
        VideoInfo* v = m_videos.takeLast();
        delete v;
    }
}

int VideoTableModel::rowCount(const QModelIndex &parent) const {
    return m_videos.size() + 1;   // +1 for empty URL placeholder
}

int VideoTableModel::columnCount(const QModelIndex &parent) const {
    return COLUMN_COUNT;
}

QVariant VideoTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    VideoInfo* videoInfo = NULL;

    if (Qt::DisplayRole == role) {
        if (index.row() < m_videos.size()) {
            videoInfo = m_videos.at(index.row());
            if (videoInfo) {
                switch (index.column()) {
                case UrlColumn:
                    return QVariant(videoInfo->url());
                    break;
                case ProgressColumn:
                    return QVariant(videoInfo->progress());
                    break;
                case StatusColumn:
                    return QVariant(videoInfo->stateString(videoInfo->state()));
                    break;
                default:
                    return QVariant();
                    break;
                }
            } else {
                return QVariant();
            }
        } else if (index.row() == m_videos.size()) {
            switch (index.column()) {
            case UrlColumn:
                return QVariant(m_addUrlString);
                break;
            default:
                return QVariant();
                break;
            }
        }
    }

    if (Qt::EditRole == role) {
        if (index.row() < m_videos.size()) {
            videoInfo = m_videos.at(index.row());
            if (videoInfo) {
                switch (index.column()) {
                case UrlColumn:
                    return QVariant(videoInfo->url());
                    break;
                default:
                    return QVariant();
                    break;
                }
            }
        }
    }

    if (Qt::UserRole == role) {
        if (index.row() < m_videos.size()) {
            videoInfo = m_videos.at(index.row());
            if (videoInfo) {
                switch (index.column()) {
                case ProgressColumn:
                    return QVariant(videoInfo->progress());
                    break;
                case StatusColumn:
                    return QVariant((int)videoInfo->state());
                    break;
                default:
                    return QVariant();
                    break;
                }

            }
        }
    }

    if ((VideoTableModel::ProgressMinimumRole == role) &&
            (index.column() == VideoTableModel::ProgressColumn) &&
            (index.row() < m_videos.size()))
    {
        videoInfo = m_videos.at(index.row());
        if (videoInfo) {
            return QVariant(videoInfo->progressMinimum());
        }
    }

    if ((VideoTableModel::ProgressMaximumRole == role) &&
            (index.column() == VideoTableModel::ProgressColumn) &&
            (index.row() < m_videos.size()))
    {
        videoInfo = m_videos.at(index.row());
        if (videoInfo) {
            return QVariant(videoInfo->progressMaximum());
        }
    }

    if ((VideoTableModel::ProgressTextRole == role) &&
            (index.column() == VideoTableModel::ProgressColumn) &&
            (index.row() < m_videos.size()))
    {
        videoInfo = m_videos.at(index.row());
        if (videoInfo) {
            return QVariant(videoInfo->progressText());
        }
    }
    return QVariant();
}

bool VideoTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid() || !value.isValid()) {
        return false;
    }
    VideoInfo* videoInfo = NULL;

    if (Qt::EditRole == role) {
        if (index.row() == m_videos.size()) {
            insertRow(index.row());
        }
        videoInfo = m_videos.at(index.row());
        if (videoInfo) {
            switch (index.column()) {
                case UrlColumn: {
                    QString url = value.toString();
                    videoInfo->setUrl(url);
                    emit dataChanged(index, index);
                }
                break;
                default:
                    return false;
                    break;
            } // switch
            return true;
        } else {
            return false;
        }
    }

    if (index.row() >= m_videos.size()) {
        return false;
    }
    videoInfo = m_videos.at(index.row());
    if (!videoInfo) {
        return false;
    }

    if (Qt::UserRole == role) {
        switch (index.column()) {
            case ProgressColumn: {
                int progress = value.toInt();
                videoInfo->setProgress(progress);
                emit dataChanged(index, index);
            }
            break;
            case StatusColumn: {
                VideoInfo::VideoState state = (VideoInfo::VideoState)value.toInt();
                if ((state >= 0) && (state < VideoInfo::STATE_COUNT)) {
                    videoInfo->setState(state);
                    emit dataChanged(index, index);
                }
            }
            break;
            default:
                return false;
                break;
        } // switch
        return true;
    }

    if ((VideoTableModel::ProgressMinimumRole == role) &&
            (index.column() == VideoTableModel::ProgressColumn))
    {
        videoInfo->setProgressMinimum(value.toInt());
        emit dataChanged(index, index);
        return true;
    }

    if ((VideoTableModel::ProgressMaximumRole == role) &&
            (index.column() == VideoTableModel::ProgressColumn))
    {
        videoInfo->setProgressMaximum(value.toInt());
        emit dataChanged(index, index);
        return true;
    }

    if ((VideoTableModel::ProgressTextRole == role) &&
            (index.column() == VideoTableModel::ProgressColumn))
    {
        videoInfo->setProgressText(value.toString());
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

QVariant VideoTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if ((Qt::Horizontal == orientation) && (Qt::DisplayRole == role)) {
        if (section < VideoTableModel::COLUMN_COUNT) {
            return QVariant(m_tableHeaderStrings[section]);
        }
    }
    return QVariant::Invalid;
}

bool VideoTableModel::insertRows(int row, int count, const QModelIndex &parent) {
    beginInsertRows(parent, row, row + count - 1);
    for (int i=0; i < count; i++) {
        VideoInfo* videoInfo = new VideoInfo();
        if (!videoInfo) {
            return false;
        }
        m_videos.insert(row + i, videoInfo);
    }
    endInsertRows();
    return true;
}

bool VideoTableModel::removeRows(int row, int count, const QModelIndex &parent) {
    if (((m_videos.size() - count) < 0) || (row >= m_videos.size())) {
        return false;
    }
    beginRemoveRows(parent, row, row + count - 1);
    for (int i=0; i < count; i++) {
        VideoInfo* videoInfo = m_videos.takeAt(row);
        if (!videoInfo) {
            return false;
        }
        delete videoInfo;
    }
    endRemoveRows();
    return true;
}

Qt::ItemFlags VideoTableModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = Qt::ItemIsEnabled;
    if (UrlColumn == index.column()) {
        flags |= Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    return flags;
}

void VideoTableModel::cleanUpEmptyLines(const QModelIndex &topLeft, const QModelIndex &bottomRight) {
    QList<int> rowsToBeRemoved;
    for (int i=topLeft.row(); i <= bottomRight.row(); i++) {
        QString url = m_videos.at(i)->url().trimmed();
        if (url.isEmpty() || url.contains(m_addUrlString)) {
            //qDebug() << "row to be cleaned: " << QString::number(i);
            rowsToBeRemoved << i;
        }
    }
    while (rowsToBeRemoved.count()) {
        removeRow(rowsToBeRemoved.takeLast());
    }
    //qDebug() << "after clean-up m_videos.size() = " << QString::number(m_videos.size());
}
