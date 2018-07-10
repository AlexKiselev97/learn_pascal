/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#include "setnewpassdialog.h"
#include "ui_setnewpassdialog.h"
#include "header.h"
#include <QTime>
#include <QCryptographicHash>

SetNewPassDialog::SetNewPassDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetNewPassDialog)
{
    ui->setupUi(this);
}

SetNewPassDialog::~SetNewPassDialog()
{
    delete ui;
}

void SetNewPassDialog::on_randButton_clicked()
{
    qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    ui->passEdit->clear();
    for (int i=0; i < 8; ++i)
        ui->passEdit->setText(ui->passEdit->text() + possibleCharacters[qrand() % possibleCharacters.size()]);
}

void SetNewPassDialog::setName(const QString& str)
{
    profileName = str;
}

void SetNewPassDialog::on_setNewPassButton_clicked()
{
    QSqlDatabase db = QSqlDatabase::database("learnPascal");
    if (db.isOpen())
    {
        auto saltyPass = ui->passEdit->text() + profileName;
        QString q = "UPDATE users SET password = \'"
                    + QCryptographicHash::hash(saltyPass.toLocal8Bit(), QCryptographicHash::Algorithm::Sha256).toHex() + "\' "
                    "WHERE login=\'" + profileName + "\';";
        QSqlQuery query(db);
        if (query.exec(q))
        {
            msgBoxSimple("Успех", "Пароль изменен");
            this->passChanged = true;
            this->close();
        }
        else
        {
            qDebug() << query.lastError().text();
            msgBoxSimple("Ошибка", "Не удалось изменить пароль");
        }
    }
}
