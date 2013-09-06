#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "config.h"
#include "Downloader.h"
#include "SubtitleLanguage.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_destDir(defaultDestDir()),
    m_updateChecker(new UpdateChecker(this)),
    m_downloader(NULL),
    m_downloadInProgress(false),
    m_exitOnSuccess(false)
{
    QDir savedDir = m_settings.value("destDir", m_destDir.absolutePath()).toString();
    if (savedDir.exists()) {
        m_destDir = savedDir;
    }

    ui->setupUi(this);

    this->setWindowTitle(this->windowTitle() + " " + tr("(v%1)").arg(YLE_DOWNLOADER_GUI_VERSION));

    initSubtitlesComboBox();

    updateDestDirLabel();
    ui->statusLabel->setText("");
    ui->cancelButton->setVisible(false);
    ui->detailsWidget->setVisible(false);
    ui->updateLabel->setVisible(false);

    layout()->setSizeConstraint(QLayout::SetFixedSize);

    m_videoTableModel = new VideoTableModel();
    if (m_videoTableModel) {
        ui->videoTableView->setModel(m_videoTableModel);
    }
    m_currentlyDownloadingVideoRow = -1;
    m_resumeDownload = false;

    m_videoTableEditTriggers = (const QFlags<QAbstractItemView::EditTrigger>)(QAbstractItemView::AllEditTriggers & ~QAbstractItemView::CurrentChanged);
    ui->videoTableView->setEditTriggers(m_videoTableEditTriggers);
    ui->videoTableView->setFocus(Qt::OtherFocusReason);

    connect(m_videoTableModel, SIGNAL(rowsRemoved(QModelIndex, int, int)),
            ui->videoTableView, SLOT(rowsRemoved(QModelIndex, int, int)));

    connect(ui->destDirButton, SIGNAL(clicked()), this, SLOT(chooseDestDir()));
    connect(ui->downloadButton, SIGNAL(clicked()), this, SLOT(startDownload()));

    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelRequested()));

    connect(ui->detailsButton, SIGNAL(toggled(bool)), ui->detailsWidget, SLOT(setVisible(bool)));

    connect(m_updateChecker, SIGNAL(updateAvailable(QString,QUrl)), this, SLOT(updateAvailable(QString,QUrl)));
    connect(ui->updateLabel, SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_videoTableModel;
}

void MainWindow::startDownloadFrom(QString url)
{
    Q_UNUSED(url)
    startDownload();
}

void MainWindow::chooseDestDir()
{
    QString newDir = QFileDialog::getExistingDirectory(this, tr("Choose download folder"), m_destDir.absolutePath());
    if (!newDir.isNull()) {
        m_destDir = QDir(newDir);
        m_settings.setValue("destDir", newDir);
        updateDestDirLabel();
    }
}

void MainWindow::saveSubtitlesChoice()
{
    m_settings.setValue("subtitles", ui->subtitlesComboBox->itemData(ui->subtitlesComboBox->currentIndex()));
}

void MainWindow::startDownload()
{
    Q_ASSERT(!m_downloadInProgress);

    if (ui->videoTableView->model()->rowCount() <= 1) {
        ui->statusLabel->setText(tr("Add videos to video list"));
        return;
    }
    ui->detailsTextEdit->clear();
    setDownloadWidgetsDisabled(true);
    ui->cancelButton->setVisible(true);

    m_currentlyDownloadingVideoRow = -1;
    startNextDownload();
}

void MainWindow::startNextDownload() {
    m_currentlyDownloadingVideoRow++;
    if (m_currentlyDownloadingVideoRow >= (ui->videoTableView->model()->rowCount() - 1)) {
        downloadEnded();
        return;
    }

    if (m_downloader) {
        m_downloader->deleteLater();
        m_downloader = NULL;
    }

    QModelIndex index = ui->videoTableView->model()->index(m_currentlyDownloadingVideoRow, VideoTableModel::UrlColumn);
    QVariant urlData = ui->videoTableView->model()->data(index);
    QUrl url = QUrl(urlData.toString());

    QStringList yleDlExtraOptions = ui->yleDlExtraOptionsLineEdit->text().trimmed().split(" ", QString::SkipEmptyParts);
    QStringList extraArgs;
    foreach (QString str, yleDlExtraOptions) {
        if (str.startsWith("-")) {
            extraArgs.append(str);
        }
    }
    if (m_resumeDownload) {
        extraArgs.append(QString(RTMPDUMP_RESUME_OPTION_STR));
    }

    m_downloader = new Downloader(url, m_destDir, this);
    m_downloader->setExtraArgs(extraArgs);

    QString subtitlesOption = ui->subtitlesComboBox->itemData(ui->subtitlesComboBox->currentIndex()).toString();
    m_downloader->setSubtitles(subtitlesOption);

    connect(m_downloader, SIGNAL(downloadSucceeded()), this, SLOT(downloadSucceeded()));
    connect(m_downloader, SIGNAL(downloadCanceled()), this, SLOT(downloadCanceled()));
    connect(m_downloader, SIGNAL(downloadFailed()), this, SLOT(downloadFailed()));

    connect(m_downloader, SIGNAL(downloadFileCreated(QString)), this, SLOT(reportDestFileName(QString)));
    connect(m_downloader, SIGNAL(downloadProgress(int)), this, SLOT(reportProgress(int)));
    connect(m_downloader, SIGNAL(downloadUnknownProgress(double)), this, SLOT(reportUnknownProgress(double)));
    connect(m_downloader, SIGNAL(downloaderOutputWritten(QString)), this, SLOT(downloaderOutputWritten(QString)));


    index = ui->videoTableView->model()->index(m_currentlyDownloadingVideoRow,
        VideoTableModel::ProgressColumn);
    ui->videoTableView->model()->setData(index, QVariant(0), VideoTableModel::ProgressMinimumRole);
    ui->videoTableView->model()->setData(index, QVariant(0), VideoTableModel::ProgressMaximumRole);
    ui->videoTableView->model()->setData(index, QVariant(-1), Qt::UserRole);
    ui->videoTableView->model()->setData(index, QVariant(""), VideoTableModel::ProgressTextRole);

    index = ui->videoTableView->model()->index(m_currentlyDownloadingVideoRow,
        VideoTableModel::StatusColumn);
    ui->videoTableView->model()->setData(index, QVariant((int)VideoInfo::StateStarting), Qt::UserRole);

    m_downloadInProgress = true;
    m_downloader->start();
}

void MainWindow::reportDestFileName(QString name)
{
    m_destFileName = name;
    ui->statusLabel->setText(tr("Downloading to file %1").arg(name));
}

void MainWindow::reportProgress(int percentage)
{
    QString pcStr = QString::number(percentage) + "%";
    QModelIndex index = ui->videoTableView->model()->index(this->m_currentlyDownloadingVideoRow,
        VideoTableModel::ProgressColumn);
    ui->videoTableView->model()->setData(index, QVariant(0), VideoTableModel::ProgressMinimumRole);
    ui->videoTableView->model()->setData(index, QVariant(100), VideoTableModel::ProgressMaximumRole);
    ui->videoTableView->model()->setData(index, QVariant(percentage), Qt::UserRole);
    ui->videoTableView->model()->setData(index, QVariant(pcStr), VideoTableModel::ProgressTextRole);

    index = ui->videoTableView->model()->index(m_currentlyDownloadingVideoRow,
        VideoTableModel::StatusColumn);
    ui->videoTableView->model()->setData(index, QVariant((int)VideoInfo::StateLoading), Qt::UserRole);
}

void MainWindow::reportUnknownProgress(double secondsDownloaded)
{

    QString progressStr = formatSecondsDownloaded(secondsDownloaded);
    QModelIndex index = ui->videoTableView->model()->index(this->m_currentlyDownloadingVideoRow,
        VideoTableModel::ProgressColumn);
    ui->videoTableView->model()->setData(index, QVariant(0), VideoTableModel::ProgressMinimumRole);
    ui->videoTableView->model()->setData(index, QVariant(0), VideoTableModel::ProgressMaximumRole);
    ui->videoTableView->model()->setData(index, QVariant(-1), Qt::UserRole);
    ui->videoTableView->model()->setData(index, QVariant(progressStr), VideoTableModel::ProgressTextRole);

    index = ui->videoTableView->model()->index(m_currentlyDownloadingVideoRow,
        VideoTableModel::StatusColumn);
    ui->videoTableView->model()->setData(index, QVariant((int)VideoInfo::StateLoading), Qt::UserRole);
}

void MainWindow::downloaderOutputWritten(QString line)
{
    ui->detailsTextEdit->appendPlainText(line);
}

void MainWindow::downloadSucceeded()
{
    QModelIndex index = ui->videoTableView->model()->index(this->m_currentlyDownloadingVideoRow,
        VideoTableModel::ProgressColumn);
    ui->videoTableView->model()->setData(index, QVariant(0), VideoTableModel::ProgressMinimumRole);
    ui->videoTableView->model()->setData(index, QVariant(100), VideoTableModel::ProgressMaximumRole);
    ui->videoTableView->model()->setData(index, QVariant(100), Qt::UserRole);
    ui->videoTableView->model()->setData(index, QVariant("100%"), VideoTableModel::ProgressTextRole);

    index = ui->videoTableView->model()->index(this->m_currentlyDownloadingVideoRow,
        VideoTableModel::StatusColumn);
    ui->videoTableView->model()->setData(index, QVariant(VideoInfo::StateLoadedOk), Qt::UserRole);

    this->startNextDownload();
}

void MainWindow::downloadCanceled()
{
    QModelIndex index = ui->videoTableView->model()->index(this->m_currentlyDownloadingVideoRow,
        VideoTableModel::StatusColumn);
    ui->videoTableView->model()->setData(index, QVariant(VideoInfo::StateCanceled), Qt::UserRole);

    index = ui->videoTableView->model()->index(this->m_currentlyDownloadingVideoRow,
        VideoTableModel::ProgressColumn);
    if (ui->videoTableView->model()->data(index, Qt::UserRole).toInt() < 0) {
        ui->videoTableView->model()->setData(index, QVariant(0), VideoTableModel::ProgressMinimumRole);
        ui->videoTableView->model()->setData(index, QVariant(100), VideoTableModel::ProgressMaximumRole);
        ui->videoTableView->model()->setData(index, QVariant(0), Qt::UserRole);
        ui->videoTableView->model()->setData(index, QVariant(""), VideoTableModel::ProgressTextRole);
    }

    downloadEnded();
}

void MainWindow::downloadFailed()
{
    QModelIndex index = ui->videoTableView->model()->index(this->m_currentlyDownloadingVideoRow,
        VideoTableModel::StatusColumn);
    ui->videoTableView->model()->setData(index, QVariant(VideoInfo::StateFailed), Qt::UserRole);

    index = ui->videoTableView->model()->index(this->m_currentlyDownloadingVideoRow,
        VideoTableModel::ProgressColumn);
    int progress = ui->videoTableView->model()->data(index, Qt::UserRole).toInt();
    if (progress < 0) {
        ui->videoTableView->model()->setData(index, QVariant(0), VideoTableModel::ProgressMinimumRole);
        ui->videoTableView->model()->setData(index, QVariant(100), VideoTableModel::ProgressMaximumRole);
        ui->videoTableView->model()->setData(index, QVariant(0), Qt::UserRole);
        ui->videoTableView->model()->setData(index, QVariant(""), VideoTableModel::ProgressTextRole);
    }

    this->startNextDownload();
}

void MainWindow::cancelRequested()
{
    if (m_downloadInProgress) {
        if (confirmCancel()) {
            if (m_downloadInProgress) {
                m_downloader->cancel();
            } else {
                qWarning() << "Downloader didn't exist when cancel requested.";
            }
        }
    } else {
        qWarning() << "Cancel requested when downloading not in progress";
    }
}

void MainWindow::updateAvailable(QString version, QUrl url)
{
    QString text = tr("An updated downloader (v%1) is available:<br/><a href=\"%2\">%2</a>")
            .arg(version)
            .arg(Qt::escape(url.toString()));
    ui->updateLabel->setText(text);
    ui->updateLabel->setVisible(true);
}

void MainWindow::openUrl(QString url)
{
    QDesktopServices::openUrl(QUrl(url));
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (m_downloadInProgress) {
        if (confirmCancel()) {
            event->accept();
        } else {
            event->ignore();
        }
    }
}

void MainWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    updateVideoTableView();
}

void MainWindow::initSubtitlesComboBox()
{
    foreach (SubtitleLanguage lang, SubtitleLanguage::getAll()) {
        ui->subtitlesComboBox->addItem(lang.displayName(), lang.optionId());
    }

    QString defaultOption = m_settings.value("subtitles", SubtitleLanguage::getDefault().optionId()).toString();
    int defaultOptionIndex = ui->subtitlesComboBox->findData(defaultOption);
    if (defaultOptionIndex != -1) {
        ui->subtitlesComboBox->setCurrentIndex(defaultOptionIndex);
    }

    connect(ui->subtitlesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(saveSubtitlesChoice()));
}

bool MainWindow::confirmCancel()
{
    QMessageBox::StandardButton choice =
            QMessageBox::question(this,
                                  this->windowTitle(),
                                  tr("Really cancel download?"),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes);

    return (choice == QMessageBox::Yes);
}

void MainWindow::downloadEnded()
{
    m_downloadInProgress = false;

    setDownloadWidgetsDisabled(false);

    ui->cancelButton->setVisible(false);
    ui->statusLabel->setText("");

    m_updateChecker->checkForUpdate();
}

void MainWindow::setDownloadWidgetsDisabled(bool disabled)
{
    ui->destDirButton->setDisabled(disabled);
    ui->yleDlExtraOptionsLineEdit->setDisabled(disabled);
    ui->downloadButton->setDisabled(disabled);
    ui->subtitlesComboBox->setDisabled(disabled);
    ui->yleDlExtraOptionsLineEdit->setDisabled(disabled);
    // Don't disable videoTableView because text will go gray. Instead,
    // disable editing.
    if (disabled) {
        ui->videoTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    } else {
        ui->videoTableView->setEditTriggers(m_videoTableEditTriggers);
    }
}

void MainWindow::updateDestDirLabel()
{
    QString folder = QDir::toNativeSeparators(m_destDir.absolutePath());
    ui->destDirLabel->setText(tr("Download folder: %1").arg(folder));
}

void MainWindow::updateVideoTableView()
{
    int urlTableWidth = ui->videoTableView->viewport()->width();
    ui->videoTableView->setColumnWidth(VideoTableModel::UrlColumn, (urlTableWidth * 3)/5);
    ui->videoTableView->setColumnWidth(VideoTableModel::ProgressColumn, urlTableWidth / 5);
    ui->videoTableView->setColumnWidth(VideoTableModel::StatusColumn, urlTableWidth / 5);
}

QString MainWindow::formatSecondsDownloaded(double secondsDownloaded)
{
    int seconds = secondsDownloaded;
    int minutes = seconds / 60;
    seconds = seconds % 60;
    if (minutes > 0) {
        return tr("%1 min %2 sec").arg(minutes, 2).arg(seconds, 2);
    } else {
        return tr("%1 sec").arg(seconds);
    }
}

QDir MainWindow::defaultDestDir()
{
    QDir dir = QDir::home();
    QStringList dlDirs = QStringList();
    dlDirs += "Downloads";
    dlDirs += "Download";
    dlDirs += tr("Downloads");
    dlDirs += tr("Download");
    dlDirs += "downloads";
    dlDirs += "download";
    dlDirs += tr("downloads");
    dlDirs += tr("download");
    dlDirs += "Desktop";
    foreach (QString dlDir, dlDirs) {
        if (dir.cd(dlDir)) {
            break;
        }
    }
    return dir;
}
