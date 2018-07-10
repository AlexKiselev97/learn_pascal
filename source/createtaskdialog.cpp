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

void CreateTaskDialog::on_createButton_clicked()
{
    if (ui->nameEdit->text().isEmpty() || ui->inputEdit->toPlainText().isEmpty()
        || ui->outputEdit->toPlainText().isEmpty() || ui->htmlEdit->text().isEmpty())
    {
        msgBoxSimple("Ошибка", "Вы должны заполнить все поля");
        return;
    }
    task.name = ui->nameEdit->text();
    task.input = ui->inputEdit->toPlainText();
    task.output = ui->outputEdit->toPlainText();
    QSqlDatabase db = QSqlDatabase::database("learnPascal");
    if (db.isOpen())
    {
        QString qCount = "SELECT TaskName "
                         "FROM task "
                         "WHERE TaskName=\'" + task.name + "\';";
        QSqlQuery query(db);
        if (!query.exec(qCount))
            qDebug() << query.lastError().text() << "\n";
        if (query.size() > 0)
        {
            msgBoxSimple("Ошибка", "Задача с таким именем уже существует");
        }
        else
        {
            QString qInsert = "INSERT INTO task (TaskName, input, answer, htmlTask) "
                        "VALUES (\'" + task.name + "\', "
                                "\'" + task.input + "\', "
                                "\'" + task.output + "\', "
                                "\'" + task.html + "\');";
            if (query.exec(qInsert))
            {
                msgBoxSimple("Успех", "Задача создана");
                success = true;
                this->close();
            }
            else
            {
                msgBoxSimple("Ошибка", "Не удалось создать задачу");
                qDebug() << qInsert << '\n' << query.lastError().text() << "\n";
            }
        }
    }
}
