/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    bool getSuccessLogin() { return successLogin; }
    Profile profile;
    ~LoginDialog();

private slots:
    void on_loginButton_clicked();

private:
    Ui::LoginDialog *ui;
    bool successLogin = false;
};

#endif // LOGINDIALOG_H
