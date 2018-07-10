/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QTreeWidget>
#include "header.h"
#include <memory>

class ProfileForm;
class LoginDialog;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    bool login();
    bool getSuccessLogin() { return successLogin; }
    void loadTaskTree();
    void init();
    ~MainWindow();

public slots:
    void continueCompile(int exitCode, QProcess::ExitStatus exitStatus);
    void continueSubmit(int exitCode, QProcess::ExitStatus exitStatus);

private slots:
    void on_action_exit_triggered();

    void on_tabWidget_currentChanged(int index);

    void on_compileButton_clicked();

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_action_openProfile_triggered();

    void on_action_exitProfile_triggered();

    void on_submitTaskButton_clicked();

    void on_action_manager_triggered();

    void on_programEdit_textChanged();

    void on_action_about_triggered();

    void on_action_help_triggered();

private:
    Ui::MainWindow *ui;
    bool successLogin = false;
    bool taskLoaded = false;
    TaskFromDB task;
    Profile profile;
    void loadProfileTasks();
    void highlightString(int str);
    bool hightlighted;
    std::unique_ptr<QProcess> proc;
    bool compileOn = false;
};

#endif // MAINWINDOW_H
