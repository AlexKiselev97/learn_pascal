/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#ifndef EDITTASKDIALOG_H
#define EDITTASKDIALOG_H

#include <QDialog>
#include "header.h"

namespace Ui {
class EditTaskDialog;
}

class EditTaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditTaskDialog(QWidget *parent = 0);
    void setTask(QString name);
    ~EditTaskDialog();

private slots:
    void on_openButton_clicked();

    void on_saveButton_clicked();

    void on_saveHtmlButton_clicked();

private:
    Ui::EditTaskDialog *ui;
    TaskFromDB task;
};

#endif // EDITTASKDIALOG_H
