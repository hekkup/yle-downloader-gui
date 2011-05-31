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

    ui->urlEdit->setFocus(Qt::OtherFocusReason);

    connect(ui->destDirButton, SIGNAL(clicked()), this, SLOT(chooseDestDir()));
    connect(ui->downloadButton, SIGNAL(clicked()), this, SLOT(startDownload()));

    connect(ui->urlEdit, SIGNAL(returnPressed()), ui->downloadButton, SLOT(click()));
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
    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(true);

    connect(m_downloader, SIGNAL(downloadSucceeded()), this, SLOT(downloadSucceeded()));
    connect(m_downloader, SIGNAL(downloadFailed()), this, SLOT(downloadFailed()));

    connect(m_downloader, SIGNAL(downloadProgress(int)), this, SLOT(reportProgress(int)));
    connect(m_downloader, SIGNAL(downloadUnknownProgress()), this, SLOT(reportUnknownProgress()));

    m_downloader->start();

    ui->statusLabel->setText("Downloading...");
}

void MainWindow::reportProgress(int percentage)
{
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(percentage);
}

void MainWindow::reportUnknownProgress()
{
    ui->progressBar->setMaximum(0);
    ui->progressBar->setValue(0);
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
