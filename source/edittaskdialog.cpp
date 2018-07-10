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

void EditTaskDialog::setTask(QString name)
{
    task.name = name;
    ui->nameEdit->setText(name);
    QSqlDatabase db = QSqlDatabase::database("learnPascal");
    if (db.isOpen())
    {
        QString q = "SELECT * FROM task "
                    "WHERE TaskName = \'" + name + "\';";
        QSqlQuery query(db);
        if (!query.exec(q))
            qDebug() << query.lastError().text() << "\n";
        else
        {
            query.next();
            task.input = query.value(1).toString();
            task.output = query.value(2).toString();
            task.html = query.value(3).toString();
            ui->inputEdit->setText(task.input);
            ui->outputEdit->setText(task.output);
            ui->htmlEdit->setText("[html файл]");
        }
    }
}

void EditTaskDialog::on_openButton_clicked()
{
    auto filename = QFileDialog::getOpenFileName(this, "Открыть HTML", "C:/", "HTML Files (*.html *htm)");
    QFile input(filename);
    if (input.open(QIODevice::ReadOnly))
    {
        task.html = input.readAll();
        task.html.replace("'", R"(\')");
        ui->htmlEdit->setText(filename);
    }
    else
        msgBoxSimple("Ошибка", "Не удается открыть файл");
}

void EditTaskDialog::on_saveButton_clicked()
{
    task.input = ui->inputEdit->toPlainText();
    task.output = ui->outputEdit->toPlainText();
    QSqlDatabase db = QSqlDatabase::database("learnPascal");
    if (db.isOpen())
    {
        QString q = "UPDATE task SET "
                    "TaskName = \'" + ui->nameEdit->text()  + "\', "
                    "input = \'" + task.input + "\', "
                    "answer = \'" + task.output + "\', "
                    "htmlTask = \'" + task.html + "\' "
                    "WHERE TaskName = \'" + task.name + "\';";
        QSqlQuery query(db);
        if (query.exec(q))
        {
            msgBoxSimple("Успех", "Изменения сохранены");
            this->close();
        }
        else
        {
            msgBoxSimple("Ошибка", "Не удалось сохранить изменения");
            qDebug() << query.lastError().text() << "\n";
        }

    }
}

void EditTaskDialog::on_saveHtmlButton_clicked()
{
    auto filename = QFileDialog::getSaveFileName(this, "Сохранить HTML", "C:/", "HTML Files (*.html)");
    QFile output(filename);
    if (output.open(QIODevice::WriteOnly))
    {
        QTextStream outStream(&output);
        outStream.setCodec("UTF-8");
        task.html.replace(R"(\')", "'");
        outStream << task.html;
        output.close();
    }
    else
        msgBoxSimple("Ошибка", "Не удается создать файл");
}
