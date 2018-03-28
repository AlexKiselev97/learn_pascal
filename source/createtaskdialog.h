/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#ifndef CREATETASKDIALOG_H
#define CREATETASKDIALOG_H

#include <QDialog>
#include "header.h"

namespace Ui {
class CreateTaskDialog;
}

class CreateTaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateTaskDialog(QWidget *parent = 0);
    QString getName() { return task.taskName; }
    bool getSuccess() { return success; }
    ~CreateTaskDialog();

private slots:
    void on_openHTML_clicked();

    void on_createButton_clicked();

private:
    Ui::CreateTaskDialog *ui;
    TaskFromDB task;
    bool success = false;
};

#endif // CREATETASKDIALOG_H
