/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#ifndef OPENPROFILEDIALOG_H
#define OPENPROFILEDIALOG_H

#include <QDialog>

namespace Ui {
class OpenProfileDialog;
}

class OpenProfileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenProfileDialog(QWidget *parent = 0);
    void setList(QStringList& strList);
    QString getProfileName() {return profileName;}
    ~OpenProfileDialog();

private slots:
    void on_openButton_clicked();

private:
    Ui::OpenProfileDialog *ui;
    QString profileName;
};

#endif // OPENPROFILEDIALOG_H
