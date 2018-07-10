/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#ifndef HEADER_H
#define HEADER_H

#include <QMainWindow>
#include <QWidget>
#include <QtSql>
#include <QMessageBox>
#include <QTreeWidget>
#include <QDir>
#include <QDebug>
#include <QProcess>

struct TaskFromDB
{
    int number;
    QString name;
    QString input;
    QString output;
    QString html;
};

struct Profile
{
    QString name;
    bool administrator = false;
    QString tasks;
};

void msgBoxSimple(QString title, QString msg);
int msgBoxQ(QString title, QString msg);

#endif // HEADER_H
