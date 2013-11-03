#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "config.h"
#include "Downloader.h"
#include "SubtitleLanguage.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_initialized(false),
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

    m_languageActionGroup = new QActionGroup(this);
    m_languageActionGroup->addAction(ui->actionFinnish);
    m_languageActionGroup->addAction(ui->actionEnglish);

    layout()->setSizeConstraint(QLayout::SetFixedSize);

    m_videoTableModel = new VideoTableModel();
    if (m_videoTableModel) {
        ui->videoTableView->setModel(m_videoTableModel);
        ui->videoTableView->setColumnHidden(VideoTableModel::FileNameColumn, true);
    }
    m_currentlyDownloadingVideoRow = -1;

#ifdef Q_OS_WIN
    m_resumeDownload = false;
#else
    m_resumeDownload = true;
#endif

    m_saveAndRestoreSession = false;

    m_videoTableEditTriggers = (const QFlags<QAbstractItemView::EditTrigger>)(QAbstractItemView::AllEditTriggers & ~QAbstractItemView::CurrentChanged);
    ui->videoTableView->setEditTriggers(m_videoTableEditTriggers);
    ui->videoTableView->setFocus(Qt::OtherFocusReason);

    setAcceptDrops(true);

    connect(m_videoTableModel, SIGNAL(rowsRemoved(QModelIndex, int, int)),
            ui->videoTableView, SLOT(rowsRemoved(QModelIndex, int, int)));

    connect(m_videoTableModel, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(videoTableRowsAdded(QModelIndex,int,int)));
    connect(m_videoTableModel, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(videoTableRowsRemoved(QModelIndex, int, int)));

    connect(ui->destDirButton, SIGNAL(clicked()), this, SLOT(chooseDestDir()));
    connect(ui->downloadButton, SIGNAL(clicked()), this, SLOT(startDownload()));

    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelRequested()));

    connect(ui->detailsButton, SIGNAL(toggled(bool)), ui->detailsWidget, SLOT(setVisible(bool)));

    connect(m_updateChecker, SIGNAL(updateAvailable(QString,QUrl)), this, SLOT(updateAvailable(QString,QUrl)));
    connect(ui->updateLabel, SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));

    connect(this, SIGNAL(currentlyDownloadingRowChanged(int)), ui->videoTableView, SLOT(changeDownloadingRow(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_videoTableModel;
}

void MainWindow::setSaveAndRestoreSession(bool saveAndRestore) {
    this->m_saveAndRestoreSession = saveAndRestore;
}

bool MainWindow::init() {
    if (m_initialized) {
        return false;
    }

    if (m_saveAndRestoreSession) {
        this->restoreSession();
    }
    if (m_videoTableModel->videoCount() > 0) {
        ui->downloadButton->setEnabled(true);
    } else {
        ui->downloadButton->setEnabled(false);
    }

    switch (this->locale().language()) {
    case QLocale::Finnish:
        ui->actionFinnish->setChecked(true);
        break;
    case QLocale::English:
        ui->actionEnglish->setChecked(true);
        break;
    default:
        break;
    }

    m_initialized = true;
    return true;
}

void MainWindow::startDownloadFrom(QString url)
{
    m_videoTableModel->addUrl(url);
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

void MainWindow::saveSession()
{
    m_settings.setValue("language", QVariant(this->locale().name()));

    m_settings.beginGroup("LastSession");
    m_settings.remove("session");
    m_settings.beginWriteArray("session");
    for (int row = 0; row < m_videoTableModel->videoCount(); row++) {
        m_settings.setArrayIndex(row);
        m_settings.setValue("URL", QVariant(m_videoTableModel->videoAt(row)->url()));
        m_settings.setValue("progress", QVariant(m_videoTableModel->videoAt(row)->progress()));
        m_settings.setValue("state", QVariant((int)m_videoTableModel->videoAt(row)->state()));
        m_settings.setValue("filename", QVariant(m_videoTableModel->videoAt(row)->fileName()));
    }
    m_settings.endArray();
    m_settings.endGroup();
}

void MainWindow::restoreSession()
{
    m_settings.beginGroup("LastSession");
    int videoCount = m_settings.beginReadArray("session");
    for (int i = 0; i < videoCount; i++) {
        m_settings.setArrayIndex(i);
        int videoRow = m_videoTableModel->addUrl(m_settings.value("URL").toString());
        if (videoRow != i) {
            qWarning() << "restored video has strange index";
        }
        int progress = m_settings.value("progress").toInt();
        if (progress < 0) {
            m_videoTableModel->setKnownDownloadProgress(videoRow, 0, false);
        } else {
            m_videoTableModel->setKnownDownloadProgress(videoRow, progress);
        }
        m_videoTableModel->setDownloadState(videoRow, VideoInfo::StateNotStarted);
        m_videoTableModel->setVideoFileName(videoRow, m_settings.value("filename").toString());
    }
    m_settings.endArray();
    m_settings.endGroup();
}

void MainWindow::videoTableRowsAdded(QModelIndex parent, int startRow, int endRow)
{
    Q_UNUSED(parent);
    Q_UNUSED(startRow);
    Q_UNUSED(endRow);
    if (!m_downloadInProgress) {
        ui->downloadButton->setEnabled(m_videoTableModel->videoCount() > 0);
    }
}

void MainWindow::videoTableRowsRemoved(QModelIndex parent, int startRow, int endRow)
{
    Q_UNUSED(parent);
    if (!m_downloadInProgress) {
        ui->downloadButton->setEnabled(m_videoTableModel->videoCount() > 0);
    } else {
        // NOTE: assuming only one row is removed at a time
        if ((startRow == endRow) &&
            (currentlyDownloadingVideoRow() > startRow))
        {
            setCurrentlyDownloadingVideoRow(currentlyDownloadingVideoRow() - 1);
        }
    }
}

void MainWindow::startDownload()
{
    Q_ASSERT(!m_downloadInProgress);

    if (m_videoTableModel->videoCount() <= 0) {
        qWarning() << "Attempt to start download with nothing to download.";
        return;
    }
    ui->detailsTextEdit->clear();
    setDownloadWidgetsDisabled(true);
    ui->cancelButton->setVisible(true);

    setCurrentlyDownloadingVideoRow(-1);
    startNextDownload();
}

void MainWindow::startNextDownload() {
    setCurrentlyDownloadingVideoRow(currentlyDownloadingVideoRow() + 1);
    if (currentlyDownloadingVideoRow() >= (m_videoTableModel->videoCount())) {
        downloadEnded();
        return;
    }

    if (m_downloader) {
        m_downloader->deleteLater();
        m_downloader = NULL;
    }

    QUrl url = QUrl(m_videoTableModel->url(currentlyDownloadingVideoRow()));

    QStringList yleDlExtraOptions = ui->yleDlExtraOptionsLineEdit->text().trimmed().split(" ", QString::SkipEmptyParts);
    QStringList extraArgs;
    foreach (QString str, yleDlExtraOptions) {
        if (str.startsWith("-")) {
            extraArgs.append(str);
        }
    }
    if (m_resumeDownload) {
        extraArgs.append(QString("-e"));
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


    m_videoTableModel->setUnknownDownloadProgress(currentlyDownloadingVideoRow(), "");
    m_videoTableModel->setDownloadState(currentlyDownloadingVideoRow(), VideoInfo::StateStarting);

    m_downloadInProgress = true;
    m_downloader->start();
}

void MainWindow::reportDestFileName(QString name)
{
    m_destFileName = name;
    ui->statusLabel->setText(tr("Downloading to file %1").arg(name));
    m_videoTableModel->setVideoFileName(currentlyDownloadingVideoRow(), name);
}

void MainWindow::reportProgress(int percentage)
{
    m_videoTableModel->setKnownDownloadProgress(currentlyDownloadingVideoRow(), percentage);
    m_videoTableModel->setDownloadState(currentlyDownloadingVideoRow(), VideoInfo::StateLoading);
}

void MainWindow::reportUnknownProgress(double secondsDownloaded)
{
    QString progressStr = formatSecondsDownloaded(secondsDownloaded);
    m_videoTableModel->setUnknownDownloadProgress(currentlyDownloadingVideoRow(), progressStr);
    m_videoTableModel->setDownloadState(currentlyDownloadingVideoRow(), VideoInfo::StateLoading);
}

void MainWindow::downloaderOutputWritten(QString line)
{
    ui->detailsTextEdit->appendPlainText(line);
}

void MainWindow::downloadSucceeded()
{
    m_videoTableModel->setKnownDownloadProgress(currentlyDownloadingVideoRow(), 100);
    m_videoTableModel->setDownloadState(currentlyDownloadingVideoRow(), VideoInfo::StateLoadedOk);

    this->startNextDownload();
}

void MainWindow::downloadCanceled()
{
    if (m_videoTableModel->downloadProgress(currentlyDownloadingVideoRow()) == -1) {
        m_videoTableModel->setKnownDownloadProgress(currentlyDownloadingVideoRow(), 0, false);
    }
    m_videoTableModel->setDownloadState(currentlyDownloadingVideoRow(), VideoInfo::StateCanceled);

    this->downloadEnded();
}

void MainWindow::downloadFailed()
{
    if (m_videoTableModel->downloadProgress(currentlyDownloadingVideoRow()) == -1) {
        m_videoTableModel->setKnownDownloadProgress(currentlyDownloadingVideoRow(), 0, false);
    }
    m_videoTableModel->setDownloadState(currentlyDownloadingVideoRow(), VideoInfo::StateFailed);

    this->startNextDownload();
}

void MainWindow::cancelRequested()
{
    if (m_downloadInProgress) {
        if (confirmCancel()) {
            if (m_downloadInProgress) {
                m_downloader->cancel();
            } else {
                qWarning() << "Download finished while confirming cancel.";
            }
        }
    } else {
        qWarning() << "Cancel requested while no download in progress.";
    }
}

void MainWindow::updateAvailable(QString version, QUrl url)
{
    QString urlstr = url.toString(); 
    QString text = tr("An updated downloader (v%1) is available:<br/><a href=\"%2\">%2</a>")
            .arg(version)
#ifdef QT_5_0
            .arg(urlstr.toHtmlEscaped());
#else
            .arg(Qt::escape(urlstr));
#endif
    ui->updateLabel->setText(text);
    ui->updateLabel->setVisible(true);
}

void MainWindow::openUrl(QString url)
{
    QDesktopServices::openUrl(QUrl(url));
}

void MainWindow::on_actionQuit_triggered(bool checked) {
    Q_UNUSED(checked);
    this->close();
}

void MainWindow::on_openDownloadFolderPushButton_clicked(bool checked) {
    Q_UNUSED(checked);
    this->openUrl(m_destDir.path());
}

void MainWindow::on_actionFinnish_triggered(bool checked) {
    Q_UNUSED(checked);
    this->setLocale(QLocale::Finnish);
}

void MainWindow::on_actionEnglish_triggered(bool checked) {
    Q_UNUSED(checked);
    this->setLocale(QLocale::English);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (m_downloadInProgress) {
        if (confirmCancel()) {
            if (m_downloadInProgress) {
                m_downloader->cancel();
            }
            event->accept();
        } else {
            event->ignore();
        }
    }
    if (event->isAccepted() && m_saveAndRestoreSession) {
        this->saveSession();
    }
}

void MainWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    updateVideoTableView();
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LocaleChange) {
        if (this->m_initialized) {
            QMessageBox message;
            message.setText(tr("Language will change after application restart"));
            message.exec();
        }
    } else {
        QMainWindow::changeEvent(event);
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event) {
    m_videoTableModel->addUrl(event->mimeData()->text());
    event->acceptProposedAction();
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
    setCurrentlyDownloadingVideoRow(-1);

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
}

void MainWindow::updateDestDirLabel()
{
    QString folder = QDir::toNativeSeparators(m_destDir.absolutePath());
    ui->destDirLabel->setText(tr("Download folder: %1").arg(folder));
}

void MainWindow::updateVideoTableView()
{
    int urlTableWidth = ui->videoTableView->viewport()->width();
    QHeaderView* header = ui->videoTableView->horizontalHeader();
    header->resizeSection(VideoTableModel::UrlColumn, urlTableWidth * 3 / 5);
    header->resizeSection(VideoTableModel::ProgressColumn, urlTableWidth / 5);
    header->resizeSection(VideoTableModel::StatusColumn, urlTableWidth / 5);
#ifdef QT_5_0
    header->setSectionResizeMode(VideoTableModel::UrlColumn, QHeaderView::Fixed);
    header->setSectionResizeMode(VideoTableModel::ProgressColumn, QHeaderView::Fixed);
    header->setSectionResizeMode(VideoTableModel::StatusColumn, QHeaderView::Fixed);
#else
    header->setResizeMode(VideoTableModel::UrlColumn, QHeaderView::Fixed);
    header->setResizeMode(VideoTableModel::ProgressColumn, QHeaderView::Fixed);
    header->setResizeMode(VideoTableModel::StatusColumn, QHeaderView::Fixed);
#endif
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

int MainWindow::currentlyDownloadingVideoRow() {
    return m_currentlyDownloadingVideoRow;
}

void MainWindow::setCurrentlyDownloadingVideoRow(int currentRow) {
    m_currentlyDownloadingVideoRow = currentRow;
    emit currentlyDownloadingRowChanged(m_currentlyDownloadingVideoRow);
}
