/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#include "createtaskdialog.h"
#include "ui_createtaskdialog.h"
#include "header.h"
#include <QFileDialog>

CreateTaskDialog::CreateTaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateTaskDialog)
{
    ui->setupUi(this);
}

CreateTaskDialog::~CreateTaskDialog()
{
    delete ui;
}

void CreateTaskDialog::on_openHTML_clicked()
{
    task.htmlTask = QFileDialog::getOpenFileName(this, tr("Открыть HTML"),
                    QDir::currentPath() + "/html", tr("HTML Files (*.html)"));
    ui->pathEdit->setText(task.htmlTask);
    auto list = task.htmlTask.split("/");
    task.htmlTask = list[list.size()-1];
}

void CreateTaskDialog::on_createButton_clicked()
{
    if (ui->nameEdit->text().isEmpty() || ui->inputEdit->toPlainText().isEmpty()
        || ui->outputEdit->toPlainText().isEmpty() || ui->pathEdit->text().isEmpty())
    {
        msgBoxSimple("Ошибка", "Вы должны заполнить все поля");
        return;
    }
    task.taskName = ui->nameEdit->text();
    task.input = ui->inputEdit->toPlainText();
    task.answer = ui->outputEdit->toPlainText();
    QString dbDir = QDir::currentPath() + "/db/task.mdb";
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=" + dbDir);
    db.setPassword("1234admin56");
    if (db.open())
    {
        QString qCount = "SELECT COUNT(table1.TaskName) "
                         "FROM table1 "
                         "WHERE (((table1.TaskName)=\'" + task.taskName + "\'));";
        QSqlQuery query(db);
        if (!query.exec(qCount))
            qDebug() << query.lastError().text() << "\n";
        query.next();
        if (query.value(0).toString() != "0")
        {
            msgBoxSimple("Ошибка", "Задача с таким именем уже существует");
        }
        else
        {
            QString qInsert = "INSERT INTO table1 ( TaskName, [input], answer, htmlTask ) "
                        "VALUES (\'" + task.taskName + "\', "
                                "\'" + task.input + "\', "
                                "\'" + task.answer + "\', "
                                "\'" + task.htmlTask + "\');";

            if (!query.exec(qInsert))
                qDebug() << query.lastError().text() << "\n";
            success = true;
            this->close();
        }
        db.close();
    }
}
