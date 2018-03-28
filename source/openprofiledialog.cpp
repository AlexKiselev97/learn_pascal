/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#include "openprofiledialog.h"
#include "ui_openprofiledialog.h"

OpenProfileDialog::OpenProfileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenProfileDialog)
{
    ui->setupUi(this);
}

void OpenProfileDialog::setList(QStringList &strList)
{
    ui->listWidget->addItems(strList);
}

OpenProfileDialog::~OpenProfileDialog()
{
    delete ui;
}

void OpenProfileDialog::on_pushButton_clicked()
{
    if (ui->listWidget->currentRow() != -1)
    {
        this->profileName = ui->listWidget->currentItem()->text();
        close();
    }
}
