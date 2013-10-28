#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#ifdef QT_5_0
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "UpdateChecker.h"
#include "VideoTableModel.h"
#include "ProgressBarDelegate.h"
#include "VideoTableView.h"

class Downloader;

namespace Ui {
    class MainWindow;
}

/**
 * @todo save/restore also extra options
 * @todo paste multi-line text: get URLs from each line
 * @todo open a text file: get URLs from each line
 * @todo at some point, add options dialog (then can remove extra options line edit)
 * @todo many rows loading, deleting all, then stopping dl -> doesn't quit properly
 */
class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setSaveAndRestoreSession(bool saveAndRestore);
    bool init();

    void startDownloadFrom(QString url);
    void setExitOnSuccess(bool value) { m_exitOnSuccess = value; }

signals:
    void currentlyDownloadingRowChanged(int currentRow);

private slots:
    void chooseDestDir();

    void saveSubtitlesChoice();
    void saveSession();
    void restoreSession();

    void videoTableRowsAdded(QModelIndex parent, int startRow, int endRow);
    void videoTableRowsRemoved(QModelIndex parent, int startRow, int endRow);

    void startDownload();
    void startNextDownload();

    void reportDestFileName(QString name);
    void reportProgress(int percentage);
    void reportUnknownProgress(double secondsDownloaded);
    void downloaderOutputWritten(QString line);
    void downloadSucceeded();
    void downloadCanceled();
    void downloadFailed();

    void cancelRequested();

    void updateAvailable(QString version, QUrl url);

    /**
     * Open URL in a suitable application. Opening application is detected automatically.
     */
    void openUrl(QString url);

    void on_actionQuit_triggered(bool checked = false);
    void on_openDownloadFolderPushButton_clicked(bool checked = false);
    void on_actionFinnish_triggered(bool checked = false);
    void on_actionEnglish_triggered(bool checked = false);

protected:
    void closeEvent(QCloseEvent* event);
    void showEvent(QShowEvent* event);
    void changeEvent(QEvent* event);

    /**
     * Tell URL dropping is possible
     */
    void dragEnterEvent(QDragEnterEvent* event);

    /**
     * Add URL to video list
     */
    void dropEvent(QDropEvent* event);

private:
    Ui::MainWindow *ui;

    bool m_initialized; ///< whether this object has been initialized or not

    QSettings m_settings;
    QDir m_destDir;
    QString m_destFileName;
    UpdateChecker* m_updateChecker;
    Downloader* m_downloader; // initially null
    QActionGroup* m_languageActionGroup;
    bool m_downloadInProgress;
    bool m_resumeDownload;  ///< whether to always resume download (yle-dl --resume|-e option)
    bool m_exitOnSuccess;
    bool m_saveAndRestoreSession;   ///< whether to save & restore session (video list + states)

    void initSubtitlesComboBox();

    bool confirmCancel();

    void downloadEnded();
    void setDownloadWidgetsDisabled(bool disabled);

    void updateDestDirLabel();
    void updateVideoTableView();

    QString formatSecondsDownloaded(double secondsDownloaded);
    static QDir defaultDestDir();

    int currentlyDownloadingVideoRow();
    void setCurrentlyDownloadingVideoRow(int currentRow);

    QNetworkAccessManager *m_networkManager;

    VideoTableModel* m_videoTableModel;
    QAbstractItemView::EditTriggers m_videoTableEditTriggers;

    int m_currentlyDownloadingVideoRow; ///< current video index in m_videoTableModel
};

#endif // MAINWINDOW_H
