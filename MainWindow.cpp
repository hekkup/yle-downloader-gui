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
    m_downloader(0),
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
    ui->progressBar->setVisible(false);
    ui->cancelButton->setVisible(false);
    ui->detailsWidget->setVisible(false);
    ui->updateLabel->setVisible(false);

    layout()->setSizeConstraint(QLayout::SetFixedSize);

    ui->urlEdit->setFocus(Qt::OtherFocusReason);

    connect(ui->destDirButton, SIGNAL(clicked()), this, SLOT(chooseDestDir()));
    connect(ui->downloadButton, SIGNAL(clicked()), this, SLOT(startDownload()));

    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelRequested()));

    connect(ui->urlEdit, SIGNAL(returnPressed()), ui->downloadButton, SLOT(click()));

    connect(ui->detailsButton, SIGNAL(toggled(bool)), ui->detailsWidget, SLOT(setVisible(bool)));

    connect(m_updateChecker, SIGNAL(updateAvailable(QString,QUrl)), this, SLOT(updateAvailable(QString,QUrl)));
    connect(ui->updateLabel, SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startDownloadFrom(QString url)
{
    ui->urlEdit->setText(url);
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

    if (m_downloader) {
        m_downloader->deleteLater();
        m_downloader = 0;
    }

    QStringList urlEditContents = ui->urlEdit->text().trimmed().split(" ", QString::SkipEmptyParts);

    QUrl url;
    QStringList extraArgs;
    // As an undocumented feature, permit options to be included in the
    foreach (QString str, urlEditContents) {
        if (str.startsWith("-")) {
            extraArgs.append(str);
        } else {
            url = QUrl(str);
        }
    }

    m_downloader = new Downloader(url, m_destDir, this);
    m_downloader->setExtraArgs(extraArgs);

    QString subtitlesOption = ui->subtitlesComboBox->itemData(ui->subtitlesComboBox->currentIndex()).toString();
    m_downloader->setSubtitles(subtitlesOption);

    setDownloadWidgetsDisabled(true);
    ui->progressBar->setMaximum(0);
    ui->progressBar->setValue(-1);
    ui->progressBar->setVisible(true);
    ui->cancelButton->setVisible(true);
    ui->detailsTextEdit->clear();

    connect(m_downloader, SIGNAL(downloadSucceeded()), this, SLOT(downloadSucceeded()));
    connect(m_downloader, SIGNAL(downloadCanceled()), this, SLOT(downloadCanceled()));
    connect(m_downloader, SIGNAL(downloadFailed()), this, SLOT(downloadFailed()));

    connect(m_downloader, SIGNAL(downloadFileCreated(QString)), this, SLOT(reportDestFileName(QString)));
    connect(m_downloader, SIGNAL(downloadProgress(int)), this, SLOT(reportProgress(int)));
    connect(m_downloader, SIGNAL(downloadUnknownProgress(double)), this, SLOT(reportUnknownProgress(double)));
    connect(m_downloader, SIGNAL(downloaderOutputWritten(QString)), this, SLOT(downloaderOutputWritten(QString)));

    ui->statusLabel->setText(tr("Starting download..."));

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
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(percentage);
}

void MainWindow::reportUnknownProgress(double secondsDownloaded)
{
    if (ui->progressBar->maximum() > 0) {
        ui->progressBar->setMaximum(0);
        ui->progressBar->setValue(-1);
    }

    QString status = tr("Downloading to file %1  (%2 downloaded)")
            .arg(m_destFileName)
            .arg(formatSecondsDownloaded(secondsDownloaded));
    ui->statusLabel->setText(status);
}

void MainWindow::downloaderOutputWritten(QString line)
{
    ui->detailsTextEdit->appendPlainText(line);
}

void MainWindow::downloadSucceeded()
{
    ui->statusLabel->setText(tr("Download finished."));
    downloadEnded(true);
    if (m_exitOnSuccess) {
        QApplication::exit();
    }
}

void MainWindow::downloadCanceled()
{
    ui->statusLabel->setText(tr("Download canceled."));
    downloadEnded(false);
}

void MainWindow::downloadFailed()
{
    ui->statusLabel->setText(tr("Download failed."));
    downloadEnded(false);
    m_updateChecker->checkForUpdate();
}

void MainWindow::cancelRequested()
{
    if (m_downloadInProgress) {
        if (confirmCancel()) {
            if (m_downloadInProgress) {
                m_downloader->cancel();
            }
        }
    } else {
        qWarning() << "Downloader didn't exist when cancel requested.";
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

void MainWindow::downloadEnded(bool success)
{
    m_downloadInProgress = false;

    setDownloadWidgetsDisabled(false);

    if (success) {
        ui->progressBar->setMaximum(100);
        ui->progressBar->setValue(100);
    } else {
        ui->progressBar->setVisible(false);
    }

    ui->cancelButton->setVisible(false);
}

void MainWindow::setDownloadWidgetsDisabled(bool disabled)
{
    ui->destDirButton->setDisabled(disabled);
    ui->urlEdit->setDisabled(disabled);
    ui->downloadButton->setDisabled(disabled);
    ui->subtitlesComboBox->setDisabled(disabled);
}

void MainWindow::updateDestDirLabel()
{
    QString folder = QDir::toNativeSeparators(m_destDir.absolutePath());
    ui->destDirLabel->setText(tr("Download folder: %1").arg(folder));
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
