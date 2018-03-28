/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#include "createprofiledialog.h"
#include "ui_createprofiledialog.h"
#include "header.h"

CreateProfileDialog::CreateProfileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateProfileDialog)
{
    ui->setupUi(this);
}

CreateProfileDialog::~CreateProfileDialog()
{
    delete ui;
}

void CreateProfileDialog::setTaskCount(int n)
{
    taskCount = n;
}

void CreateProfileDialog::on_createProfileButton_clicked()
{
    if (ui->loginEdit->text().size() > 0)
    {
        if (ui->passEdit->text().size() > 0)
        {
            QString dbDir = QDir::currentPath() + "/db/users.mdb";
            QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
            db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=" + dbDir);
            db.setPassword("1234admin56");
            if (db.open())
            {
                QString qCount = "SELECT COUNT(users.login) "
                            "FROM users "
                            "WHERE (((users.login)=\'" + ui->loginEdit->text() + "\'));";
                QSqlQuery query(db);
                if (!query.exec(qCount))
                    qDebug() << query.lastError().text() << "\n";
                else
                {
                    if (query.value(0).toString() == "0")
                    {
                        int adminChecked = ui->checkBox->isChecked() ? -1 : 0;
                        QString q = "INSERT INTO users (login, password, admin, tasks) "
                            "VALUES (\'" + ui->loginEdit->text() + "\', "
                                    "\'" + ui->passEdit->text() + "\', "
                                     + QString::number(adminChecked) + ", "
                                    "\'" + QString(taskCount, '0') + "\');";
                        if (!query.exec(q))
                            qDebug() << query.lastError().text() << "\n";
                        this->close();
                    }
                    else
                    {
                        msgBoxSimple("Ошибка", "Пользователь с таким именем уже существует.");
                    }
                }
                db.close();
            }
            else
                qDebug() << db.lastError().text() << "\n";
        }
        else
        {
            msgBoxSimple("Ошибка", "Введите пароль пользователя.");
        }
    }
    else
    {
        msgBoxSimple("Ошибка", "Введите имя пользователя.");
    }

}
