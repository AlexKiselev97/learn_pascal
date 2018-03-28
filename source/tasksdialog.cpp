/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#include "tasksdialog.h"
#include "ui_tasksdialog.h"
#include "createtaskdialog.h"
#include "edittaskdialog.h"

TasksDialog::TasksDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TasksDialog)
{
    ui->setupUi(this);
}

void TasksDialog::setTask(QTreeWidget *twp)
{
    for (int i = 0; i < twp->topLevelItemCount(); ++i)
    {
       QTreeWidgetItem* item = twp->topLevelItem(i);
       ui->treeWidget->addTopLevelItem(item->clone());
    }
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
    {
        ui->treeWidget->topLevelItem(i)->setFlags(ui->treeWidget->topLevelItem(i)->flags() | Qt::ItemIsEditable);
        for (int j = 0; j < ui->treeWidget->topLevelItem(i)->childCount(); ++j)
        {
            auto* child = ui->treeWidget->topLevelItem(i)->child(j);
            child->setIcon(0, QIcon());
            child->setFlags(child->flags() | Qt::ItemIsEditable);
        }
    }
}

TasksDialog::~TasksDialog()
{
    delete ui;
}

void TasksDialog::saveTree()
{
    QString dbDir = QDir::currentPath() + "/db/taskTree.mdb";
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=" + dbDir);
    db.setPassword("1234admin56");
    if (db.open())
    {
        QSqlQuery query(db);
        QString qDrop = "DROP TABLE table1';";
        if (!query.exec(qDrop))
            qDebug() << query.lastError().text() << "\n";
        QString qCreate = "CREATE TABLE table1 (Код COUNTER PRIMARY KEY, Чей_ребенок INT, Название TEXT);";
        if (!query.exec(qCreate))
            qDebug() << query.lastError().text() << "\n";
        int count = 0;
        for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
        {
            ++count;
            QString qInsertParent = "INSERT INTO table1 ( Чей_ребенок, Название ) "
                              "VALUES (0, \'" + ui->treeWidget->topLevelItem(i)->text(0) + "\');";
            if (!query.exec(qInsertParent))
                qDebug() << query.lastError().text() << "\n";
            int parentNumber = count;
            for (int j = 0; j < ui->treeWidget->topLevelItem(i)->childCount(); ++j)
            {
                ++count;
                QString qInsertChild = "INSERT INTO table1 ( Чей_ребенок, Название ) "
                                  "VALUES (" + QString::number(parentNumber) + ", "
                                  "\'" + ui->treeWidget->topLevelItem(i)->child(j)->text(0) + "\');";
                if (!query.exec(qInsertChild))
                    qDebug() << query.lastError().text() << "\n";
            }
        }
        db.close();
    }
}

void TasksDialog::closeEvent(QCloseEvent* event)
{
    saveTree();
}

void TasksDialog::on_createSectionButton_clicked()
{
    auto* item = new QTreeWidgetItem;
    item->setText(0, "Новый раздел");
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    ui->treeWidget->addTopLevelItem(item);
    saveTree();
}

void TasksDialog::on_createTaskButton_clicked()
{
    if (ui->treeWidget->currentItem() == 0)
        return;
    if (ui->treeWidget->currentItem()->parent() == 0)
    {
        CreateTaskDialog ctdp;
        ctdp.exec();
        if (ctdp.getSuccess())
        {
            auto* item = new QTreeWidgetItem;
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            item->setText(0, ctdp.getName());
            ui->treeWidget->currentItem()->addChild(item);
            addEmptyTaskToAllProfiles(numberOfTask(item));
            saveTree();
        }
    }
}

void TasksDialog::on_deleteSectionButton_clicked()
{
    if (ui->treeWidget->currentItem() == 0)
        return;
    if (ui->treeWidget->currentItem()->parent() == 0)
    {
        auto ans = msgBoxQ("Подтвердите действие",
                           "Вы действительно хотите удалить раздел? (Раздел будет удален вместе со ВСЕМИ задачи в нем)");
        if (ans == QMessageBox::Yes)
        {
            while (ui->treeWidget->currentItem()->childCount() > 0)
            {
                deleteTask(ui->treeWidget->currentItem()->child(0));
            }
            delete ui->treeWidget->currentItem();
        }
    }
}

void TasksDialog::deleteTask(QTreeWidgetItem* item)
{
    QString dbDir = QDir::currentPath() + "/db/task.mdb";
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=" + dbDir);
    db.setPassword("1234admin56");
    if (db.open())
    {
        QSqlQuery query(db);
        QString q = "DELETE * FROM table1 WHERE TaskName =\'" + item->text(0) + "\';";
        if (!query.exec(q))
            qDebug() << query.lastError().text() << "\n";
        db.close();
    }
    deleteTaskToAllProfiles(numberOfTask(item));
    delete item;
    saveTree();
}

void TasksDialog::on_deleteTaskButton_clicked()
{
    if (ui->treeWidget->currentItem() == 0)
        return;
    if (ui->treeWidget->currentItem()->parent() != 0)
    {
        auto ans = msgBoxQ("Подтвердите действие", "Вы действительно хотите удалить задачу?");
        if (ans == QMessageBox::Yes)
        {
            deleteTask(ui->treeWidget->currentItem());
        }
    }
}

void TasksDialog::moveTask(int pos, QString side)
{
    QString dbDir = QDir::currentPath() + "/db/users.mdb";
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=" + dbDir);
    db.setPassword("1234admin56");
    if (db.open())
    {
        QSqlQuery query(db);
        QString qGetLoginAndTasks = "SELECT login, tasks FROM users;";
        QList<QPair<QString, QString>> taskList;
        if (!query.exec(qGetLoginAndTasks))
            qDebug() << query.lastError().text() << "\n";
        else
        {
            while (query.next())
                taskList.push_back(qMakePair(query.value(0).toString(), query.value(1).toString()));
            for (auto& pair: taskList)
            {
                if (side == "up")
                {
                    QChar temp = pair.second[pos];
                    pair.second[pos] = pair.second[pos-1];
                    pair.second[pos-1] = temp;
                }
                else
                {
                    QChar temp = pair.second[pos];
                    pair.second[pos] = pair.second[pos+1];
                    pair.second[pos+1] = temp;
                }
                QString qUpdateTask = "UPDATE users SET users.tasks = \'" + pair.second + "\' "
                                      "WHERE (((users.login)=\'" + pair.first + "\'));";
                query.exec(qUpdateTask);
            }
        }
        db.close();
    }
}

void TasksDialog::moveSection(int pos, int len, int insertPos, QString side)
{
    QString dbDir = QDir::currentPath() + "/db/users.mdb";
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=" + dbDir);
    db.setPassword("1234admin56");
    if (db.open())
    {
        QSqlQuery query(db);
        QString qGetLoginAndTasks = "SELECT login, tasks FROM users;";
        QList<QPair<QString, QString>> taskList;
        if (!query.exec(qGetLoginAndTasks))
            qDebug() << query.lastError().text() << "\n";
        else
        {
            while (query.next())
                taskList.push_back(qMakePair(query.value(0).toString(), query.value(1).toString()));
            for (auto& pair: taskList)
            {
                if (side == "up")
                {
                    QString temp = pair.second.mid(pos, len);
                    pair.second.remove(pos, len);
                    pair.second.insert(insertPos, temp);
                }
                else
                {
                    QString temp = pair.second.mid(pos, len);
                    pair.second.insert(insertPos+1, temp);
                    pair.second.remove(pos, len);
                }
                QString qUpdateTask = "UPDATE users SET users.tasks = \'" + pair.second + "\' "
                                      "WHERE (((users.login)=\'" + pair.first + "\'));";
                query.exec(qUpdateTask);
            }
        }
        db.close();
    }
}

void TasksDialog::on_upItemButton_clicked()
{
    if (ui->treeWidget->currentItem() == 0)
        return;
    if (ui->treeWidget->currentItem()->parent() == 0)
    {
        if (ui->treeWidget->currentIndex().row() > 0)
        {
            if (ui->treeWidget->currentItem()->childCount() > 0)
            {
                int pos = numberOfTask(ui->treeWidget->currentItem()->child(0));
                int len = ui->treeWidget->currentItem()->childCount();
                int insertPos = pos - ui->treeWidget->topLevelItem(ui->treeWidget->currentIndex().row()-1)->childCount();
                moveSection(pos, len, insertPos, "up");
            }
            int index = ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->currentItem());
            QTreeWidgetItem* item = ui->treeWidget->takeTopLevelItem(index);
            ui->treeWidget->insertTopLevelItem(index-1, item);
        }
    }
    else
    {
        if (ui->treeWidget->currentIndex().row() > 0)
        {
            moveTask(numberOfTask(ui->treeWidget->currentItem()), "up");
            QTreeWidgetItem* parent = ui->treeWidget->currentItem()->parent();
            int index = parent->indexOfChild(ui->treeWidget->currentItem());
            QTreeWidgetItem* child = parent->takeChild(index);
            parent->insertChild(index-1, child);
        }
    }
    saveTree();
}

void TasksDialog::on_downItemButton_clicked()
{
    if (ui->treeWidget->currentItem() == 0)
        return;
    if (ui->treeWidget->currentItem()->parent() == 0)
    {
        if (ui->treeWidget->currentIndex().row() < ui->treeWidget->topLevelItemCount() - 1)
        {
            if (ui->treeWidget->currentItem()->childCount() > 0)
            {
                int pos = numberOfTask(ui->treeWidget->currentItem()->child(0));
                int len = ui->treeWidget->currentItem()->childCount();
                int insertPos = pos + len;
                moveSection(pos, len, insertPos, "down");
            }
            int index = ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->currentItem());
            QTreeWidgetItem* item = ui->treeWidget->takeTopLevelItem(index);
            ui->treeWidget->insertTopLevelItem(index+1, item);
        }
    }
    else
    {
        if (ui->treeWidget->currentIndex().row() < ui->treeWidget->currentItem()->parent()->childCount() - 1)
        {
            moveTask(numberOfTask(ui->treeWidget->currentItem()), "down");
            QTreeWidgetItem* parent = ui->treeWidget->currentItem()->parent();
            int index = parent->indexOfChild(ui->treeWidget->currentItem());
            QTreeWidgetItem* child = parent->takeChild(index);
            parent->insertChild(index+1, child);
        }
    }
    saveTree();
}

void TasksDialog::addEmptyTaskToAllProfiles(int pos)
{
    QString dbDir = QDir::currentPath() + "/db/users.mdb";
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=" + dbDir);
    db.setPassword("1234admin56");
    if (db.open())
    {
        QString qAllTask = "SELECT users.login, users.tasks FROM users;";
        QSqlQuery query(db);
        std::vector<std::pair<QString, QString>> usersList;
        if (!query.exec(qAllTask))
            qDebug() << query.lastError().text() << "\n";
        else
            while (query.next())
                usersList.push_back(std::make_pair(query.value(0).toString(), query.value(1).toString()));
        for (auto& pair: usersList)
        {
            qDebug() << "pair second b4 insert " << pair.second;
            pair.second.insert(pos, QChar('0'));
            qDebug() << "pair second after insert " << pair.second;
            QString qUpdateTasks = "UPDATE users SET users.tasks = \'" + pair.second + "\' "
                                   "WHERE (((users.login)=\'" + pair.first + "\'));";
            if (!query.exec(qUpdateTasks))
                qDebug() << query.lastError().text() << "\n";
        }
        db.close();
    }
}

void TasksDialog::deleteTaskToAllProfiles(int pos)
{
    QString dbDir = QDir::currentPath() + "/db/users.mdb";
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=" + dbDir);
    db.setPassword("1234admin56");
    if (db.open())
    {
        QString qAllTask = "SELECT users.login, users.tasks FROM users;";
        QSqlQuery query(db);
        std::vector<std::pair<QString, QString>> usersList;
        if (!query.exec(qAllTask))
            qDebug() << query.lastError().text() << "\n";
        else
            while (query.next())
                usersList.push_back(std::make_pair(query.value(0).toString(), query.value(1).toString()));
        for (auto& pair: usersList)
        {
            qDebug() << "pair second b4 remove " << pair.second;
            pair.second.remove(pos, 1);
            qDebug() << "pair second after remove " << pair.second;
            QString qUpdateTasks = "UPDATE users SET users.tasks = \'" + pair.second + "\' "
                                   "WHERE (((users.login)=\'" + pair.first + "\'));";
            if (!query.exec(qUpdateTasks))
                qDebug() << query.lastError().text() << "\n";
        }
        db.close();
    }
}

void TasksDialog::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    oldName = item->text(0);
}

void TasksDialog::on_treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    if (item->parent() != 0 && !oldName.isNull())
    {
        QString dbDir = QDir::currentPath() + "/db/task.mdb";
        QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
        db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=" + dbDir);
        db.setPassword("1234admin56");
        if (db.open())
        {
            QString q = "UPDATE table1 SET table1.TaskName = \'" + item->text(0) + "\' "
                        "WHERE (((table1.TaskName)=\'" + oldName + "\'));";
            QSqlQuery query(db);
            if (!query.exec(q))
                qDebug() << query.lastError().text() << "\n";
        }
        oldName = "";
    }
}

int TasksDialog::numberOfTask(QTreeWidgetItem* item)
{
    int count = 0;
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
        for (int j = 0; j < ui->treeWidget->topLevelItem(i)->childCount(); ++j)
            if (ui->treeWidget->topLevelItem(i)->child(j) != item)
                ++count;
            else
                return count;
}

void TasksDialog::on_editTaskButton_clicked()
{
    if (ui->treeWidget->currentItem() == 0)
        return;
    if (ui->treeWidget->currentItem()->parent() != 0)
    {
        EditTaskDialog etdp;
        etdp.setTaskName(ui->treeWidget->currentItem()->text(0));
        etdp.exec();
    }

}
