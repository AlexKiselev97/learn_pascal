/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#ifndef SETNEWPASSDIALOG_H
#define SETNEWPASSDIALOG_H

#include <QDialog>

namespace Ui {
class SetNewPassDialog;
}

class SetNewPassDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetNewPassDialog(QWidget *parent = 0);
    void setName(const QString& str);
    bool passChanged = false;
    ~SetNewPassDialog();

private slots:
    void on_randButton_clicked();

    void on_setNewPassButton_clicked();

private:
    Ui::SetNewPassDialog *ui;
    QString profileName;
};

#endif // SETNEWPASSDIALOG_H
