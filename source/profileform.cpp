/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#include "profileform.h"
#include "ui_profileform.h"
#include "openprofiledialog.h"
#include "createprofiledialog.h"
#include "setnewpassdialog.h"

ProfileForm::ProfileForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProfileForm)
{
    ui->setupUi(this);
}

ProfileForm::~ProfileForm()
{
    delete ui;
}

void ProfileForm::askSaveProfile()
{
    if (profileWasChanged)
    {
        auto ans = msgBoxQ("Потвердите действие", "Сохранить изменения в профиле?");
        if (ans == QMessageBox::Yes)
        {
            this->on_saveProfile_clicked();
        }
        profileWasChanged = false;
    }
}

void ProfileForm::closeEvent(QCloseEvent *event)
{
    askSaveProfile();
}

void ProfileForm::setProfile(const Profile& p, QTreeWidget* twp)
{
    profileWasChanged = false;
    ui->loginEdit->setText(p.name);
    adminProfileName = p.name;
    ui->statusEdit->setText(p.administrator ? "Администратор" : "Пользователь");
    this->profileTasks = p.tasks;
    ui->treeWidget->clear();
    for (int i = 0; i < twp->topLevelItemCount(); ++i)
    {
       QTreeWidgetItem* item = twp->topLevelItem(i);
       ui->treeWidget->addTopLevelItem(item->clone());
    }
    ui->deleteProfileButton->setVisible(p.administrator);
    ui->openProfileButton->setVisible(p.administrator);
    ui->createProfileButton->setVisible(p.administrator);
    ui->saveProfile->setVisible(p.administrator);
    ui->changeTaskStatusButton->setVisible(p.administrator);
    ui->changePassButton->setVisible(p.administrator);
}

void ProfileForm::on_openProfileButton_clicked()
{
    askSaveProfile();
    QSqlDatabase db = QSqlDatabase::database("learnPascal");
    QStringList loginList;
    if (db.isOpen())
    {
        QString q = "SELECT login FROM users;";
        QSqlQuery query(db);
        if (!query.exec(q))
            qDebug() << query.lastError().text() << "\n";
        else
        {
            while (query.next())
                loginList.push_back(query.value(0).toString());
        }
    }
    else
        qDebug() << db.lastError().text() << "\n";
    OpenProfileDialog ofd;
    ofd.setList(loginList);
    ofd.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    ofd.exec();
    auto profileName = ofd.getProfileName();
    if (profileName == ui->loginEdit->text())
    {
        msgBoxSimple("Ошибки", "Вы уже в этом профиле");
    }
    else
    {
        QString q = "SELECT login, admin, tasks FROM users "
                    "WHERE login= \'" + profileName +"\';";
        QSqlQuery query(db);
        if (!query.exec(q))
            qDebug() << query.lastError().text() << "\n";
        else
        {
            if (query.next())
            {
                ui->loginEdit->setText(query.value(0).toString());
                ui->statusEdit->setText(query.value(1).toBool() ? "Администратор" : "Пользователь");
                profileTasks = query.value(2).toString();
                int count = 0;
                for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
                {
                   QTreeWidgetItem* item = ui->treeWidget->topLevelItem(i);
                   for (int j=0; j < item->childCount(); ++j)
                   {
                       if(profileTasks[count] == '0' && !item->child(j)->icon(0).isNull())
                       {
                           item->child(j)->setIcon(0, QIcon());
                       }
                       else if(profileTasks[count] == '1' && item->child(j)->icon(0).isNull())
                       {
                           item->child(j)->setIcon(0, QIcon(":/images/ok-512.png"));
                       }
                       ++count;
                   }
                }
                profileWasChanged = false;
            }
            else
            {
                msgBoxSimple("Ошибка", "Не найдена запись профиля " + profileName + ".");
            }
        }
    }
}

void ProfileForm::on_changeTaskStatusButton_clicked()
{
    if (ui->treeWidget->currentIndex().row() != -1)
    {
        if (ui->treeWidget->currentItem()->parent() != 0)
        {
            if(ui->treeWidget->currentItem()->icon(0).isNull())
            {
                ui->treeWidget->currentItem()->setIcon(0, QIcon(":/images/ok-512.png"));
            }
            else
            {
                ui->treeWidget->currentItem()->setIcon(0, QIcon());
            }
            profileWasChanged = true;
        }
    }
}

void ProfileForm::on_saveProfile_clicked()
{
    profileTasks = "";
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
    {
       QTreeWidgetItem* item = ui->treeWidget->topLevelItem(i);
       for (int j=0; j < item->childCount(); ++j)
       {
           if(item->child(j)->icon(0).isNull())
           {
               profileTasks.push_back('0');
           }
           else
               profileTasks.push_back('1');
       }
    }
    QSqlDatabase db = QSqlDatabase::database("learnPascal");
    if (db.isOpen())
    {
        QString q = "UPDATE users SET tasks = \'" + this->profileTasks + "\' "
                    "WHERE login=\'" + ui->loginEdit->text() + "\';";
        QSqlQuery query(db);
        if (!query.exec(q))
            qDebug() << query.lastError().text() << "\n";
    }
    else
        qDebug() << db.lastError().text() << "\n";

}

void ProfileForm::on_createProfileButton_clicked()
{
    askSaveProfile();
    CreateProfileDialog cpd;
    cpd.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    cpd.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    int taskCount = 0;
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
       taskCount += ui->treeWidget->topLevelItem(i)->childCount();
    cpd.setTaskCount(taskCount);
    cpd.exec();
}

void ProfileForm::on_deleteProfileButton_clicked()
{
    if (adminProfileName == ui->loginEdit->text())
    {
        msgBoxSimple("Ошибка", "Вы не можете удалить сами себя. Откройте другой профиль.");
        return;
    }
    QSqlDatabase db = QSqlDatabase::database("learnPascal");
    if (!db.isOpen())
    {
        return;
    }
    auto ans = msgBoxQ("Подтвердите действие",
        "Вы действительно хотите удалить пользователя под именем " + ui->loginEdit->text() + "?");
    if (ans == QMessageBox::Yes)
    {
        QString q = "DELETE FROM users "
            "WHERE login=\'" + ui->loginEdit->text() + "\';";
        QSqlQuery query(db);
        if (!query.exec(q))
            qDebug() << query.lastError().text() << "\n";
        else
        { // загрузка профиля админа
            QString q = "SELECT login, admin, tasks FROM users "
                        "WHERE login= \'" + adminProfileName +"\';";
            QSqlQuery query(db);
            if (!query.exec(q))
                qDebug() << query.lastError().text() << "\n";
            else
            {
                if (query.next())
                {
                    ui->loginEdit->setText(query.value(0).toString());
                    ui->statusEdit->setText(query.value(1).toBool() ? "Администратор" : "Пользователь");
                    profileTasks = query.value(2).toString();
                    int count = 0;
                    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
                    {
                       QTreeWidgetItem* item = ui->treeWidget->topLevelItem(i);
                       for (int j=0; j < item->childCount(); ++j)
                       {
                           if(profileTasks[count] == '0' && !item->child(j)->icon(0).isNull())
                           {
                               item->child(j)->setIcon(0, QIcon());
                           }
                           else if(profileTasks[count] == '1' && item->child(j)->icon(0).isNull())
                           {
                               item->child(j)->setIcon(0, QIcon(":/images/ok-512.png"));
                           }
                           ++count;
                       }
                    }
                }
                else
                {
                    msgBoxSimple("Ошибка", "Не найдена запись профиля " + ui->loginEdit->text() + ".");
                }
            }
        }
    }
}

void ProfileForm::on_changePassButton_clicked()
{
    SetNewPassDialog dialog;
    dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    dialog.setName(this->ui->loginEdit->text());
    dialog.exec();
}
