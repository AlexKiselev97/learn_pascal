/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#include "logindialog.h"
#include "ui_logindialog.h"
#include "mainwindow.h"
#include "header.h"

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
    QString dbDir = QDir::currentPath() + "/db/users.mdb";
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=" + dbDir);
    db.setPassword("1234admin56");
    if (db.open())
    {
        QString q = "SELECT users.[login], users.[password], users.[admin], users.[tasks] FROM users "
                    "WHERE (((users.[login])= \'" + ui->loginEdit->text() +"\') "
                    "AND ((users.[password])=\'" + ui->passEdit->text() + "\'));";
        QSqlQuery query(db);
        if (!query.exec(q))
            qDebug() << query.lastError().text() << "\n";
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
        db.close();
    }
    else
        qDebug() << db.lastError().text() << "\n";
}
