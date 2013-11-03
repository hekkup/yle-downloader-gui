#ifndef VIDEOINFO_H
#define VIDEOINFO_H

#include <QObject>

/**
 * Video info class, used as items in VideoTableModel.
 */
class VideoInfo : public QObject {
    Q_OBJECT
public:

    enum VideoState {
        StateNotStarted = 0,
        StateStarting,
        StateLoading,
        StateLoadedOk,
        StateCanceled,
        StateFailed,
        STATE_COUNT
    };

    explicit VideoInfo(QObject *parent = 0);
    ~VideoInfo();

    /**
     * Get URL of video
     */
    QString url();

    /**
     * Get current download progress percentage
     */
    int progress();

    int progressMinimum();

    int progressMaximum();

    QString progressText();

    /**
     * Get video download state
     */
    VideoInfo::VideoState state();

    /**
     * Get state string corresponding to download state
     */
    QString stateString(VideoInfo::VideoState);

    /**
     * Get target file name
     */
    QString fileName();

    /**
     * Set video URL
     */
    void setUrl(QString url);

    /**
     * Set video download progress
     * @deprecated
     */
    void setProgress(int progress);

    /**
     * Set progress bar minimum value
     */
    void setProgressMinimum(int progressMinimum);

    /**
     * Set progress bar maximum value
     */
    void setProgressMaximum(int progressMaximum);

    /**
     * Set text to be displayed on progress bar
     */
    void setProgressText(QString text);

    /**
     * Set status of video download
     */
    void setState(VideoInfo::VideoState state);

    /**
     * Set target file name
     */
    void setFileName(QString fileName);

#ifndef UNIT_TEST
private:
#endif
    QString m_url;
    int m_progress;
    int m_progressMinimum;
    int m_progressMaximum;
    QString m_progressText;
    VideoState m_videoState;
    QString m_videoStateStrings[VideoInfo::STATE_COUNT];
    QString m_fileName;
};

#endif // VIDEOINFO_H
