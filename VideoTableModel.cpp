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

int VideoTableModel::addUrl(QString url) {
    QString trimmedStr = url.trimmed();
    if (trimmedStr.isEmpty()) {
        return -1;
    }
    int row = rowCount() - 1;
    QModelIndex index = this->index(row, VideoTableModel::UrlColumn);
    if (setData(index, QVariant(url), Qt::EditRole)) {
        return row;
    } else {
        return -1;
    }
}

QString VideoTableModel::url(int row) {
    if (row >= this->m_videos.size()) {
        return QString("");
    }
    QModelIndex index = this->index(row, VideoTableModel::UrlColumn);
    QVariant dataItem = this->data(index, Qt::DisplayRole);
    if (dataItem.isValid()) {
        return dataItem.toString();
    } else {
        return QString("");
    }
}

bool VideoTableModel::setKnownDownloadProgress(int row, int progress, bool showText) {
    QModelIndex index = this->index(row, VideoTableModel::ProgressColumn);
    QString text;
    if (showText) {
        text = QString::number(progress) + "%";
    } else {
        text = "";
    }
    bool ok = true;
    if (!setData(index, QVariant(0), VideoTableModel::ProgressMinimumRole)) { ok = false; }
    if (!setData(index, QVariant(100), VideoTableModel::ProgressMaximumRole)) { ok = false; }
    if (!setData(index, QVariant(progress), VideoTableModel::ProgressRole)) { ok = false; }
    if (!setData(index, QVariant(text), VideoTableModel::ProgressTextRole)) { ok = false; }
    return ok;
}

bool VideoTableModel::setUnknownDownloadProgress(int row, QString text) {
    QModelIndex index = this->index(row, VideoTableModel::ProgressColumn);
    bool ok = true;
    if (!setData(index, QVariant(0), VideoTableModel::ProgressMinimumRole)) { ok=false; }
    if (!setData(index, QVariant(0), VideoTableModel::ProgressMaximumRole)) { ok=false; }
    if (!setData(index, QVariant(-1), VideoTableModel::ProgressRole)) { ok=false; }
    if (!setData(index, QVariant(text), VideoTableModel::ProgressTextRole)) { ok=false; }
    return ok;
}

int VideoTableModel::downloadProgress(int row) {
    QModelIndex index = this->index(row, VideoTableModel::ProgressColumn);
    QVariant dataItem = this->data(index, VideoTableModel::ProgressRole);
    if (dataItem.isValid()) {
        return dataItem.toInt();
    }
    return 0;
}

bool VideoTableModel::setDownloadState(int row, VideoInfo::VideoState state) {
    QModelIndex index = this->index(row, VideoTableModel::StatusColumn);
    if (!setData(index, QVariant((int)state), Qt::UserRole)) {
        return false;
    }
    return true;
}

bool VideoTableModel::setVideoFileName(int row, QString fileName) {
    QModelIndex index = this->index(row, VideoTableModel::FileNameColumn);
    if (!setData(index, QVariant(fileName), Qt::EditRole)) {
        return false;
    }
    return true;
}

VideoInfo* VideoTableModel::videoAt(int row) {
    if ((row < 0) || (row >= m_videos.size())) {
        return NULL;
    }
    return m_videos.at(row);
}

int VideoTableModel::videoCount() {
    return m_videos.size();
}

int VideoTableModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return m_videos.size() + 1;   // +1 for empty URL placeholder
}

int VideoTableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
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
                case FileNameColumn:
                    return QVariant(videoInfo->fileName());
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

    if (Qt::ToolTipRole == role) {
        if (index.row() >= m_videos.size()) {
            return QVariant();
        }
        if (index.column() == UrlColumn) {
            videoInfo = m_videos.at(index.row());
            if (!videoInfo) {
                return QVariant();
            }
            QString videoFileName = videoInfo->fileName().trimmed();
            QString tooltipString = tr("Video file name") + QString("\n");
            if (videoFileName.isEmpty()) {
                tooltipString += tr("<unknown>");
            } else {
                tooltipString += videoFileName;
            }
            return QVariant(tooltipString);
        }
        return QVariant();
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

    if ((Qt::UserRole == role) || (VideoTableModel::ProgressRole == role)) {
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
        switch (index.column()) {
            case UrlColumn: {
                if (index.row() == m_videos.size()) {
                    insertRow(index.row());
                }
                videoInfo = m_videos.at(index.row());
                if (!videoInfo) {
                    return false;
                }
                QString url = value.toString();
                videoInfo->setUrl(url);
                emit dataChanged(index, index);
            }
            break;
            case FileNameColumn: {
            if (index.row() >= m_videos.size()) {
                    return false;
                }
                videoInfo = m_videos.at(index.row());
                if (!videoInfo) {
                    return false;
                }
                videoInfo->setFileName(value.toString());
                emit dataChanged(index, index);
            }
            break;
            default:
                return false;
                break;
        } // switch
        return true;
    }

    if (index.row() >= m_videos.size()) {
        return false;
    }
    videoInfo = m_videos.at(index.row());
    if (!videoInfo) {
        return false;
    }

    if ((Qt::UserRole == role) || (VideoTableModel::ProgressRole == role)) {
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
        m_videos.insert(row + i, videoInfo);
    }
    endInsertRows();
    return true;
}

bool VideoTableModel::removeRows(int row, int count, const QModelIndex &parent) {
    if (row < 0 || count < 0 || row + count > m_videos.size()) {
        return false;
    }
    beginRemoveRows(parent, row, row + count - 1);
    for (int i=0; i < count; i++) {
        delete m_videos.takeAt(row);
    }
    endRemoveRows();
    return true;
}

Qt::ItemFlags VideoTableModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = Qt::ItemIsEnabled;
    if (UrlColumn == index.column()) {
        flags |= Qt::ItemIsSelectable;
        QModelIndex stateIndex = this->index(index.row(), VideoTableModel::StatusColumn);
        VideoInfo::VideoState state = (VideoInfo::VideoState)this->data(stateIndex, Qt::UserRole).toInt();
        // enable editing when video download is not yet started or it failed
        if ((VideoInfo::StateNotStarted == state) || (VideoInfo::StateFailed == state)) {
            flags |= Qt::ItemIsEditable;
        }
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
