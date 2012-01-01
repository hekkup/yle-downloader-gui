#include "YlePassiDialog.h"
#include "ui_YlePassiDialog.h"

YlePassiDialog::YlePassiDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::YlePassiDialog)
{
    ui->setupUi(this);
}

YlePassiDialog::~YlePassiDialog()
{
    delete ui;
}

QString YlePassiDialog::username()
{
    return ui->usernameField->text();
}

QString YlePassiDialog::password()
{
    return ui->passwordField->text();
}
