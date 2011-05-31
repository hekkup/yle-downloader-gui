#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Downloader.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_destDir(QDir::current())
{
    QDir savedDir = m_settings.value("destDir", QDir::current().absolutePath()).toString();
    if (savedDir.exists()) {
        m_destDir = savedDir;
    }

    ui->setupUi(this);

    ui->destDirLabel->setText("Lataushakemisto: " + m_destDir.absolutePath());
    ui->statusLabel->setText("");

    connect(ui->destDirButton, SIGNAL(clicked()), this, SLOT(chooseDestDir()));
    connect(ui->downloadButton, SIGNAL(clicked()), this, SLOT(startDownload()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::chooseDestDir()
{
    QString newDir = QFileDialog::getExistingDirectory(this, QString("Valitse lataushakemisto"), m_destDir.absolutePath());
    if (!newDir.isNull()) {
        m_destDir = QDir(newDir);
        m_settings.setValue("destDir", newDir);
        ui->destDirLabel->setText("Lataushakemisto: " + m_destDir.absolutePath());
    }
}

void MainWindow::startDownload()
{
    QUrl url = QUrl(ui->urlEdit->text());
    Downloader* downloader = new Downloader(url, m_destDir, this);

    ui->downloadButton->setDisabled(true);
    ui->progressBar->setValue(0);

    connect(downloader, SIGNAL(downloadSucceeded()), this, SLOT(downloadEnded()));
    connect(downloader, SIGNAL(downloadFailed()), this, SLOT(downloadEnded()));

    connect(downloader, SIGNAL(downloadProgress(int)), ui->progressBar, SLOT(setValue(int)));

    downloader->start();

    ui->statusLabel->setText("Ladataan...");
}

void MainWindow::downloadEnded()
{
    ui->downloadButton->setDisabled(false);

    ui->statusLabel->setText("Lataus valmis.");
}
