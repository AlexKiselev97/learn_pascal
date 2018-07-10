/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#include "logindialog.h"
#include "ui_logindialog.h"
#include "mainwindow.h"
#include "header.h"
#include <QCryptographicHash>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_loginButton_clicked()
{
    QSqlDatabase db = QSqlDatabase::database("learnPascal");
    if (db.isOpen())
    {
        auto saltyPass = ui->passEdit->text() + ui->loginEdit->text();
        QString q = "SELECT login, password, admin, tasks FROM users "
                    "WHERE (login= \'" + ui->loginEdit->text() +"\') "
                    "AND (password=\'"
                    + QCryptographicHash::hash(saltyPass.toLocal8Bit(), QCryptographicHash::Algorithm::Sha256).toHex() + "\');";
        QSqlQuery query(db);
        if (!query.exec(q))
            qDebug() << query.lastError().text();
        else
        {
            if (query.next())
            {
                profile.name = ui->loginEdit->text();
                profile.administrator = query.value(2).toBool();
                profile.tasks = query.value(3).toString();
                successLogin = true;
                this->close();
            }
            else
            {
                msgBoxSimple("Ошибка", "Неправильный логин или пароль");
            }
        }
    }
    else
        qDebug() << db.lastError().text();
}
