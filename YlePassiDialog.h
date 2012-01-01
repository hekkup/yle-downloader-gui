#ifndef YLEPASSIDIALOG_H
#define YLEPASSIDIALOG_H

#include <QDialog>

namespace Ui {
    class YlePassiDialog;
}

class YlePassiDialog: public QDialog
{
    Q_OBJECT
    
public:
    explicit YlePassiDialog(QWidget *parent = 0);
    ~YlePassiDialog();
    
    QString username();
    QString password();

private:
    Ui::YlePassiDialog *ui;
};

#endif // YLEPASSIDIALOG_H
