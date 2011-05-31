#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtGui>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void chooseDestDir();

    void startDownload();
    void downloadEnded();

private:
    Ui::MainWindow *ui;

    QSettings m_settings;
    QDir m_destDir;
};

#endif // MAINWINDOW_H
