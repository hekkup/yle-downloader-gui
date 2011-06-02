#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Downloader.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_destDir(QDir::current()),
    m_downloader(0)
{
    QDir savedDir = m_settings.value("destDir", QDir::current().absolutePath()).toString();
    if (savedDir.exists()) {
        m_destDir = savedDir;
    }

    ui->setupUi(this);

    updateDestDirLabel();
    ui->statusLabel->setText("");
    ui->progressBar->setVisible(false);
    ui->detailsWidget->setVisible(false);

    layout()->setSizeConstraint(QLayout::SetFixedSize);

    ui->urlEdit->setFocus(Qt::OtherFocusReason);

    connect(ui->destDirButton, SIGNAL(clicked()), this, SLOT(chooseDestDir()));
    connect(ui->downloadButton, SIGNAL(clicked()), this, SLOT(startDownload()));

    connect(ui->urlEdit, SIGNAL(returnPressed()), ui->downloadButton, SLOT(click()));

    connect(ui->detailsButton, SIGNAL(toggled(bool)), ui->detailsWidget, SLOT(setVisible(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::startDownload()
{
    QUrl url = QUrl(ui->urlEdit->text());

    if (m_downloader) {
        delete m_downloader;
    }

    m_downloader = new Downloader(url, m_destDir, this);

    setDownloadWidgetsDisabled(true);
    ui->progressBar->setMaximum(0);
    ui->progressBar->setValue(-1);
    ui->progressBar->setVisible(true);

    connect(m_downloader, SIGNAL(downloadSucceeded()), this, SLOT(downloadSucceeded()));
    connect(m_downloader, SIGNAL(downloadFailed()), this, SLOT(downloadFailed()));

    connect(m_downloader, SIGNAL(downloadFileCreated(QString)), this, SLOT(reportDestFileName(QString)));
    connect(m_downloader, SIGNAL(downloadProgress(int)), this, SLOT(reportProgress(int)));
    connect(m_downloader, SIGNAL(downloadUnknownProgress(double)), this, SLOT(reportUnknownProgress(double)));
    connect(m_downloader, SIGNAL(downloaderOutputWritten(QString)), this, SLOT(downloaderOutputWritten(QString)));

    ui->statusLabel->setText(tr("Starting download..."));

    m_downloader->start();
}

void MainWindow::reportDestFileName(QString name)
{
    destFileName = name;
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
            .arg(destFileName)
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
    downloadEnded();
}

void MainWindow::downloadFailed()
{
    ui->statusLabel->setText(tr("Download failed."));
    downloadEnded();
}

void MainWindow::downloadEnded()
{
    setDownloadWidgetsDisabled(false);

    if (ui->progressBar->maximum() == 0) {
        ui->progressBar->setVisible(false);
    } else {
        ui->progressBar->setValue(100);
    }

    delete m_downloader;
    m_downloader = 0;
}

void MainWindow::setDownloadWidgetsDisabled(bool disabled)
{
    ui->destDirButton->setDisabled(disabled);
    ui->urlEdit->setDisabled(disabled);
    ui->downloadButton->setDisabled(disabled);
}

void MainWindow::updateDestDirLabel()
{
    ui->destDirLabel->setText(tr("Download folder: %1").arg(m_destDir.absolutePath()));
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
