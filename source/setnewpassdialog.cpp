/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#include "setnewpassdialog.h"
#include "ui_setnewpassdialog.h"
#include "header.h"
#include <QTime>

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
    QString dbDir = QDir::currentPath() + "/db/users.mdb";
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=" + dbDir);
    db.setPassword("1234admin56");
    if (db.open())
    {
        QString q = "UPDATE users SET users.password = \'" + ui->passEdit->text() + "\' "
                    "WHERE (((users.login)=\'" + profileName + "\'));";
        QSqlQuery query(db);
        if (!query.exec(q))
            qDebug() << query.lastError().text() << "\n";
        db.close();
        this->passChange = true;
        this->close();
    }
}
