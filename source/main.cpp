/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("fusion");
    MainWindow w;
    w.setMinimumHeight(w.height()/3*2);
    w.setMinimumWidth(w.height());
    if (w.getSuccessLogin())
        return a.exec();
    return 0;
}

void msgBoxSimple(QString title, QString msg)
{
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    msgBox.setButtonText(QMessageBox::Yes, "Да");
    msgBox.setButtonText(QMessageBox::No, "Нет");
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
