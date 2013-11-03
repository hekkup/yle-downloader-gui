#ifndef VIDEOTABLEMODEL_H
#define VIDEOTABLEMODEL_H

#include "VideoInfo.h"
#include <QObject>
#include <QAbstractTableModel>
#include <QList>
#include <QSize>
#include <QDebug>

/**
 * @brief data model for VideoTableView
 */
class VideoTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    /**
     * Columns of the model / table
     */
    enum Column {
        UrlColumn = 0,
        ProgressColumn,
        StatusColumn,
        FileNameColumn,
        COLUMN_COUNT
    };

    /**
     * Progress column related data roles for data() & setData()
     */
    enum VideoInfoItemRoles {
        ProgressRole = Qt::UserRole,
        ProgressMinimumRole,
        ProgressMaximumRole,
        ProgressTextRole,
        VideoInfoItemRoles_COUNT
    };

    explicit VideoTableModel(QObject *parent = 0);
    ~VideoTableModel();
    
    /**
     * Add URL to model.
     * @return row number if success, -1 if failed (empty or placeholder string)
     */
    int addUrl(QString url);

    /**
     * Get URL from row. Placeholder text from the bottom-most row is not
     * returned.
     * @return URL/text if row exists, or empty string if row doesn't exist
     */
    QString url(int row);

    /**
     * Set a known download progress (percentage).
     * @param showText Whether to set progress text for progress bar
     */
    bool setKnownDownloadProgress(int row, int progress, bool showText = true);

    /**
     * Set an unknown download progress with text.
     */
    bool setUnknownDownloadProgress(int row, QString text);

    /**
     * Get download progress. If the progress was recently set with
     * setUnknownDownloadProgress(), the progress is -1, otherwise it's the
     * set progress.
     * @return progress if it's known, -1 if it's unknown
     */
    int downloadProgress(int row);

    /**
     * Set download state.
     */
    bool setDownloadState(int row, VideoInfo::VideoState state);

    bool setVideoFileName(int row, QString fileName);

    /**
     * Get pointer to video at row position
     * @return valid pointer if exists, NULL if not
     */
    VideoInfo* videoAt(int row);

    /**
     * Get video count. Note that this is rowCount() - 1.
     */
    int videoCount();

    /**
     * Get table row count + 1. The one extra row is for internal purposes:
     * to show the "<add URL>" text in the bottom-most row.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * Get data. Note that StatusColumn gives string value when role is
     * Qt::DisplayRole. In order to get a numerical value, use Qt::UserRole.
     * For progress column, use roles in VideoInfoItemRoles.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    /**
     * Set data. Note that you have to use Qt::UserRole in order to set
     * values for StatusColumn. For UrlColumn the role must
     * be Qt::EditRole. StatusColumn only accepts numerical values, i.e.
     * VideoInfo::VideoState.
     * For progress column, use roles in VideoInfoItemRoles.
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    Qt::ItemFlags flags(const QModelIndex &index) const;

// Some bug in Qt: this won't work after signals & slots
#ifndef UNIT_TEST
private:
#endif
    QList<VideoInfo*> m_videos;
    QString m_tableHeaderStrings[VideoTableModel::COLUMN_COUNT];
    QString m_addUrlString;

public slots:
    void cleanUpEmptyLines(const QModelIndex &topLeft, const QModelIndex &bottomRight);
};

#endif // VIDEOTABLEMODEL_H
