/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#include "edittaskdialog.h"
#include "ui_edittaskdialog.h"
#include <QFileDialog>

EditTaskDialog::EditTaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditTaskDialog)
{
    ui->setupUi(this);
}

EditTaskDialog::~EditTaskDialog()
{
    delete ui;
}

void EditTaskDialog::setTaskName(QString name)
{
    task.taskName = name;
    ui->nameEdit->setText(name);
    QString dbDir = QDir::currentPath() + "/db/task.mdb";
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=" + dbDir);
    db.setPassword("1234admin56");
    if (db.open())
    {
        QString q = "SELECT * FROM table1 "
                    "WHERE TaskName = \'" + name + "\';";
        QSqlQuery query(db);
        if (!query.exec(q))
            qDebug() << query.lastError().text() << "\n";
        else
        {
            query.next();
            task.input = query.value(1).toString();
            task.answer = query.value(2).toString();
            task.htmlTask = query.value(3).toString();
            ui->inputEdit->setText(task.input);
            ui->outputEdit->setText(task.answer);
            ui->htmlEdit->setText(task.htmlTask);
        }
    }
}

void EditTaskDialog::on_openButton_clicked()
{
    task.htmlTask = QFileDialog::getOpenFileName(this, tr("Открыть HTML"),
                    QDir::currentPath() + "/html", tr("HTML Files (*.html)"));
    ui->htmlEdit->setText(task.htmlTask);
    QStringList list = task.htmlTask.split("/");
    task.htmlTask = list[list.size()-1];
}

void EditTaskDialog::on_saveButton_clicked()
{
    task.input = ui->inputEdit->toPlainText();
    task.answer = ui->outputEdit->toPlainText();
    QString dbDir = QDir::currentPath() + "/db/task.mdb";
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=" + dbDir);
    db.setPassword("1234admin56");
    if (db.open())
    {
        QString q = "UPDATE table1 SET "
                    "TaskName = \'" + ui->nameEdit->text()  + "\', "
                    "[input] = \'" + task.input + "\', "
                    "answer = \'" + task.answer + "\', "
                    "htmlTask = \'" + task.htmlTask + "\' "
                    "WHERE TaskName = \'" + task.taskName + "\';";
        QSqlQuery query(db);
        if (!query.exec(q))
            qDebug() << query.lastError().text() << "\n";
        db.close();
    }
}
