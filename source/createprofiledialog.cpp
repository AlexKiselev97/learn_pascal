/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#include "createprofiledialog.h"
#include "ui_createprofiledialog.h"
#include "header.h"
#include <QCryptographicHash>

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
    taskAmount = n;
}

void CreateProfileDialog::on_createProfileButton_clicked()
{
    if (ui->loginEdit->text().size() <= 0)
    {
        msgBoxSimple("Ошибка", "Введите имя пользователя.");
    }
    else
        if (ui->passEdit->text().size() <= 0)
        {
            msgBoxSimple("Ошибка", "Введите пароль пользователя.");
        }
        else
        {
            QSqlDatabase db = QSqlDatabase::database("learnPascal");
            if (db.isOpen())
            {
                QString qCount = "SELECT login "
                                 "FROM users "
                                 "WHERE login=\'" + ui->loginEdit->text() + "\';";
                QSqlQuery query(db);
                if (!query.exec(qCount))
                    qDebug() << query.lastError().text() << "\n";
                else
                {
                    if (query.size() > 0)
                    {
                        msgBoxSimple("Ошибка", "Пользователь с таким именем уже существует.");
                    }
                    else
                    {
                        int adminChecked = ui->checkBox->isChecked() ? 1 : 0;
                        auto saltyPass = ui->passEdit->text() + ui->loginEdit->text();
                        QString q = "INSERT INTO users (login, password, admin, tasks) "
                            "VALUES (\'" + ui->loginEdit->text() + "\', "
                                    "\'" + QCryptographicHash::hash(saltyPass.toLocal8Bit(), QCryptographicHash::Algorithm::Sha256).toHex() + "\', "
                                     + QString::number(adminChecked) + ", "
                                    "\'" + QString(taskAmount, '0') + "\');";
                        if (query.exec(q))
                        {
                            msgBoxSimple("Успех", "Профиль создан");
                            this->close();
                        }
                        else
                        {
                            msgBoxSimple("Ошибка", "Не удалось создать профиль");
                            qDebug() << query.lastError().text() << "\n";
                        }
                    }
                }
            }
            else
                qDebug() << db.lastError().text() << "\n";
        }
}
