/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#ifndef TASKSDIALOG_H
#define TASKSDIALOG_H

#include <QDialog>
#include "header.h"

namespace Ui {
class TasksDialog;
}

class TasksDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TasksDialog(QWidget *parent = 0);
    void setTask(QTreeWidget* twp);
    ~TasksDialog();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void on_createSectionButton_clicked();

    void on_createTaskButton_clicked();

    void on_deleteSectionButton_clicked();

    void on_deleteTaskButton_clicked();

    void on_upItemButton_clicked();

    void on_downItemButton_clicked();

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_treeWidget_itemChanged(QTreeWidgetItem *item, int column);

    void on_editTaskButton_clicked();

private:
    Ui::TasksDialog *ui;
    void addEmptyTaskToAllProfiles(int pos);
    void deleteTaskToAllProfiles(int pos);
    void deleteTask(QTreeWidgetItem* item);
    int numberOfTask(QTreeWidgetItem* item);
    void moveTask(int pos, QString side);
    void moveSection(int pos, int len, int insertPos, QString side);
    void saveTree();
    QString oldName;
};

#endif // TASKSDIALOG_H
