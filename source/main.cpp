/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#include "mainwindow.h"
#include <QApplication>
#include <QSqlDriver>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("fusion");
    MainWindow w;
    w.setMinimumHeight(w.height()/3*2);
    w.setMinimumWidth(w.height());
    QFile connectFile("connect.txt");
    QList<QByteArray> connectData;
    if (connectFile.open(QIODevice::ReadOnly))
    {
        connectData = connectFile.readAll().split(' ');
    }
    else
    {
        msgBoxSimple("Ошибка", "Не удается прочитать данные для соединения с сервером");
        qDebug() << connectFile.errorString();
        return 0;
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", "learnPascal");
    db.setDatabaseName("learnPascal");
    db.setUserName(connectData.at(0));
    db.setPassword(connectData.at(1));
    db.setHostName(connectData.at(2));
    if (!db.open())
    {
        msgBoxSimple("Ошибка", "Не удается подключиться к серверу");
        qDebug() << db.lastError().text() << "\n";
        return 0;
    }
    w.init();
    if (w.getSuccessLogin())
        return a.exec();
    return 0;
}

void msgBoxSimple(QString title, QString msg)
{
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    msgBox.setWindowTitle(title);
    msgBox.setText(msg);
    msgBox.exec();
}

int msgBoxQ(QString title, QString msg)
{
    QMessageBox msgBox;
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    msgBox.setButtonText(QMessageBox::Yes, "Да");
    msgBox.setButtonText(QMessageBox::No, "Нет");
    msgBox.setWindowTitle(title);
    msgBox.setText(msg);
    return msgBox.exec();
}
