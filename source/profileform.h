/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#ifndef PROFILEFORM_H
#define PROFILEFORM_H

#include <QWidget>
#include "header.h"

namespace Ui {
class ProfileForm;
}

class ProfileForm : public QDialog
{
    Q_OBJECT

public:
    explicit ProfileForm(QWidget *parent = 0);
    void setProfile(const Profile& p, QTreeWidget* twp);
    ~ProfileForm();

protected:
    void closeEvent(QCloseEvent *event) override;
private slots:
    void on_openProfileButton_clicked();

    void on_changeTaskStatusButton_clicked();

    void on_saveProfile_clicked();

    void on_createProfileButton_clicked();

    void on_deleteProfileButton_clicked();

    void on_changePassButton_clicked();

private:
    Ui::ProfileForm *ui;
    void askSaveProfile();
    QString adminProfileName;
    QString profileTasks;
    bool profileWasChanged = false;
};

#endif // PROFILEFORM_H
