/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#ifndef CREATEPROFILEDIALOG_H
#define CREATEPROFILEDIALOG_H

#include <QDialog>

namespace Ui {
class CreateProfileDialog;
}

class CreateProfileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateProfileDialog(QWidget *parent = 0);
    void setTaskCount(int n);
    ~CreateProfileDialog();

private slots:
    void on_createProfileButton_clicked();

private:
    Ui::CreateProfileDialog *ui;
    int taskCount;
};

#endif // CREATEPROFILEDIALOG_H
