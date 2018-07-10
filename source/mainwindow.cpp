/////////////////////////////////
// Learn Pascal
// Copyright © 2018 Alex Kiselev
/////////////////////////////////

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "header.h"
#include "logindialog.h"
#include "profileform.h"
#include "tasksdialog.h"
#include "aboutdialog.h"
#include <QTextStream>
#include <QTextCodec>
#include <QDesktopWidget>
#include <QTimer>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    delete ui->mainToolBar;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadTaskTree()
{
    ui->treeWidget->clear();
    QSqlDatabase db = QSqlDatabase::database("learnPascal");
    if (db.isOpen())
    {
        QString q = "SELECT * FROM taskTree;";
        QSqlQuery query(db);
        if (!query.exec(q))
            qDebug() << q << "\n" << query.lastError().text() << "\n";
        else
        {
            int count = 0;
            while (query.next())
            {
                int whosChild = query.value(1).toInt();
                QString name = query.value(2).toString();
                auto* item = new QTreeWidgetItem;
                if (whosChild == 0)
                {
                    item->setText(0, name);
                    ui->treeWidget->addTopLevelItem(item);
                }
                else
                {
                    item->setText(0, name);
                    if (this->profile.tasks.size() > count)
                        if (profile.tasks[count] == QChar('1'))
                            item->setIcon(0, QIcon(":/images/ok-512.png"));
                    QSqlQuery findParent(db);
                    QString qFindParent = "SELECT Код, Название "
                                          "FROM taskTree "
                                          "WHERE Код=" + QString::number(whosChild) + ";";
                    if (!findParent.exec(qFindParent))
                        qDebug() << qFindParent << "\n" << findParent.lastError().text() << "\n";
                    else
                    {
                        findParent.next();
                        auto parent = ui->treeWidget->findItems(findParent.value(1).toString(), Qt::MatchExactly);
                        parent.front()->addChild(item);
                    }
                    ++count;
                }
            }
        }
    }
    else
        qDebug() << db.lastError().text() << "\n";
}

void MainWindow::loadProfileTasks()
{
    QSqlDatabase db = QSqlDatabase::database("learnPascal");
    if (db.isOpen())
    {
        QString q = "SELECT tasks "
                    "FROM users "
                    "WHERE login=\'" + this->profile.name + "\';";
        QSqlQuery query(db);
        if (!query.exec(q))
            qDebug() << q << "\n" << query.lastError().text() << "\n";
        else
            if (query.next())
                profile.tasks = query.value(0).toString();
    }
}

bool MainWindow::login()
{
    LoginDialog loginDialog;
    loginDialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    loginDialog.move(QApplication::desktop()->screenGeometry().width()/2 - loginDialog.geometry().width()/2,
                    QApplication::desktop()->screenGeometry().height()/2 - loginDialog.geometry().height()/2);
    loginDialog.exec();
    if (loginDialog.getSuccessLogin())
    {
        this->profile = loginDialog.profile;
        this->showMaximized();
        return true;
    }
    return false;
}

void MainWindow::init()
{
    ui->tabWidget->setCurrentIndex(0);
    this->setVisible(false);
    ui->treeWidget->clear();
    successLogin = login();
    if (successLogin)
    {
        loadTaskTree();
        taskLoaded = false;
        ui->tab_task->setEnabled(false);
    }
    else
        this->close();
}

void MainWindow::continueCompile(int exitCode, QProcess::ExitStatus exitStatus)
{
    QTextCodec* cp866 = QTextCodec::codecForName("IBM 866");
    QString programAnswer = cp866->toUnicode(proc->readAllStandardOutput());
    QString programError = cp866->toUnicode(proc->readAllStandardError());
    proc->close();
    if (exitStatus == QProcess::ExitStatus::CrashExit)
    {
        ui->statusBar->showMessage("Ошибка времени исполнения");
        msgBoxSimple("Ошибка исполнения", "Вышло время ожидания программы (10 секунд).\nПодсказка: возможно, программа ожидала ввод данных.");
    }
    else if (!programError.isEmpty())
    {
        ui->statusBar->showMessage("Ошибка времени исполнения");
        int index = programError.indexOf("sourcePascal.pas:строка") + 24;
        QString n;
        while (programError[index] != '\r')
            n.push_back(programError[index++]);
        highlightString(n.toInt());
        programError = programError.split("\n").at(1);
    }
    else
        ui->statusBar->showMessage("Программа завершилась успешно");
    ui->outputEdit->setText(programError + programAnswer);
    compileOn = false;
    proc.reset(nullptr); // удаляем процесс
}

void MainWindow::on_compileButton_clicked()
{
    if (compileOn)
        return;
    compileOn = true;
    ui->statusBar->showMessage("Компиляция...");
    ui->outputEdit->clear();
    QString program = ui->programEdit->toPlainText();
    program.replace("\n", "\r\n");  // новая строка в windows
    QString backupDir = QDir::currentPath();
    QDir::setCurrent(QDir::currentPath() + "/pascalCompiler");
    QFile source("sourcePascal.pas");
    if (source.open(QIODevice::WriteOnly))
    {
        QTextStream sourceStream(&source);
        sourceStream << program;
        source.close();
    }
    else
    {
        msgBoxSimple("Ошибка", "Не удается сохранить исходный код");
        QDir::setCurrent(backupDir);
        return;
    }
    // компилируем
    proc.reset(new QProcess);
    proc->start("pabcnetcclear.exe", QStringList() << "sourcePascal.pas");
    proc->waitForFinished();
    QString compileOutput = proc->readAllStandardOutput();
    proc->close();
    if (compileOutput == "OK\r\n")
    {
        ui->statusBar->showMessage("Запуск программы...");
        // запускаем бинарный файл
        connect(proc.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
              this, &this->continueCompile);
        proc->start("sourcePascal.exe");
        if (ui->inputEdit->toPlainText().size() > 0)
        {
            proc->write((ui->inputEdit->toPlainText().toStdString() + "\n").data());
            proc->closeWriteChannel();
        }
        QTimer::singleShot(10000, proc.get(), &proc->kill); // ожидание выполнения 10 секунд
    }
    else
    {
        ui->statusBar->showMessage("Ошибка компиляции");
        // извлекается номер строки с ошибкой
        int n = compileOutput.at(compileOutput.indexOf("[")+1).unicode() - '0';
        highlightString(n);
        msgBoxSimple("Ошибка компиляции", compileOutput);
        compileOn = false;
    }
    QDir::setCurrent(backupDir);
}

void MainWindow::continueSubmit(int exitCode, QProcess::ExitStatus exitStatus)
{
    QTextCodec* cp866 = QTextCodec::codecForName("IBM 866");
    QString programOutput = cp866->toUnicode(proc->readAllStandardOutput());
    QString programError = cp866->toUnicode(proc->readAllStandardError());
    proc->close();
    programOutput.replace("\r\n", "\n");
    if (programOutput.endsWith("\n"))
        programOutput.chop(1);
    if (exitStatus == QProcess::ExitStatus::CrashExit)
    {
        ui->statusBar->showMessage("Ошибка времени исполнения");
        msgBoxSimple("Ошибка исполнения", "Вышло время ожидания программы (10 секунд).\nПодсказка: возможно, программа ожидала ввод данных.");
    }
    else if (!programError.isEmpty())
    {
        ui->statusBar->showMessage("Ошибка времени исполнения");
        int index = programError.indexOf("sourcePascal.pas:строка") + 24; // позиция номера строки с ошибкой
        QString n;
        while (programError[index] != '\r')
            n.push_back(programError[index++]);
        highlightString(n.toInt());
        programError = programError.split("\n").at(1);
    }
    else
    {
        ui->statusBar->showMessage("Программа завершилась успешно");
        if (programOutput == task.output)
        {
            this->profile.tasks[this->task.number] = '1';
            auto childList = ui->treeWidget->findItems(task.name, Qt::MatchExactly | Qt::MatchRecursive, 0);
            childList.front()->setIcon(0, QIcon(":/images/ok-512.png"));
            this->profile.tasks = "";
            for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
            {
               QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);
               for (int j=0; j < item->childCount(); ++j)
               {
                   if(item->child(j)->icon(0).isNull())
                       this->profile.tasks.push_back('0');
                   else
                       this->profile.tasks.push_back('1');
               }
            }
            QSqlDatabase db = QSqlDatabase::database("learnPascal");
            if (db.isOpen())
            {
                QString q = "UPDATE users SET tasks = \'" + this->profile.tasks + "\' "
                            "WHERE login=\'" + this->profile.name + "\';";
                QSqlQuery query(db);
                if (!query.exec(q))
                    qDebug() << q << "\n" << query.lastError().text() << "\n";
            }
            else
                qDebug() << db.lastError().text() << "\n";
            msgBoxSimple("Результат", "Поздравляем! Вы верно решили задачу.");
        }
        else
        {
            msgBoxSimple("Результат", "Тестовое задание решено неверно");
            qDebug() << programOutput << "\n" << task.output;
        }
    }
    ui->outputEdit->setText(programError);
    compileOn = false;
    proc.reset(nullptr); // удаляем процесс
}

void MainWindow::on_submitTaskButton_clicked()
{
    if (compileOn)
        return;
    compileOn = true;
    ui->statusBar->showMessage("Компиляция...");
    QString program = ui->programEdit->toPlainText();
    program.replace("\n", "\r\n"); // новая строка в windows
    QString backupDir = QDir::currentPath();
    QDir::setCurrent(QDir::currentPath() + "/pascalCompiler");
    QFile source("sourcePascal.pas");
    if (source.open(QIODevice::WriteOnly))
    {
        QTextStream sourceStream(&source);
        sourceStream << program;
        source.close();
    }
    else
    {
        msgBoxSimple("Ошибка", "Не удается сохранить исходный код");
        QDir::setCurrent(backupDir);
        return;
    }
    // компилируем
    proc.reset(new QProcess);
    proc->start("pabcnetcclear.exe", QStringList() << "sourcePascal.pas");
    proc->waitForFinished();
    QString compileOutput = proc->readAllStandardOutput();
    proc->close();
    if (compileOutput == "OK\r\n")
    {
        ui->statusBar->showMessage("Запуск программы...");
        connect(proc.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                          this, &this->continueSubmit);
        proc->start("sourcePascal.exe");
        if (task.input.size() > 0)
        {
            proc->write((task.input.toStdString() + "\n").data());
            proc->closeWriteChannel();
        }
        QTimer::singleShot(10000, proc.get(), &proc->kill); // ожидание выполнения 10 секунд
    }
    else
    {
        ui->statusBar->showMessage("Ошибка компиляции");
        // извлекается номер строки с ошибкой
        int n = compileOutput.at(compileOutput.indexOf("[")+1).unicode() - '0';
        highlightString(n);
        msgBoxSimple("Ошибка компиляции", compileOutput);
        compileOn = false;
    }
    QDir::setCurrent(backupDir);
}

void MainWindow::on_action_exit_triggered()
{
    this->close();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index == 1 && taskLoaded)
    {
        auto size = ui->textBrowser->document()->size();
        ui->textBrowser->setMinimumHeight(size.height()+5);
    }
}

void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (item->parent() != 0)
    {
        QSqlDatabase db = QSqlDatabase::database("learnPascal");
        if (db.isOpen())
        {
            QString filename = item->text(column);
            QString q = "SELECT * FROM task WHERE taskName = \'" + filename + "\';";
            QSqlQuery query(db);
            if (!query.exec(q))
                qDebug() << q << "\n" << query.lastError().text();
            else
            {
                if (query.next())
                {
                    task.name = query.value(0).toString();
                    task.input = query.value(1).toString();
                    task.output = query.value(2).toString();
                    task.html = query.value(3).toString();
                    task.html.replace(R"(\')", "'");
                    ui->textBrowser->setHtml(task.html);
                    taskLoaded = true;
                    ui->tab_task->setEnabled(true);
                    ui->tabWidget->setCurrentIndex(1);
                    ui->programEdit->setText("");
                    ui->inputEdit->setText("");
                    ui->outputEdit->setText("");
                    task.number = -1;
                    int count = 0;
                    for (int i=0; i < ui->treeWidget->topLevelItemCount() && task.number == -1; ++i)
                        for (int j=0; j < ui->treeWidget->topLevelItem(i)->childCount() && task.number == -1; ++j)
                            if (ui->treeWidget->topLevelItem(i)->child(j)->text(0) == task.name)
                                task.number = count;
                            else
                                count++;
                }
            }
        }
        else
            qDebug() << db.lastError().text();
    }
}

void MainWindow::on_action_openProfile_triggered()
{
    ProfileForm form;
    form.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    form.setProfile(this->profile, this->ui->treeWidget);
    form.exec();
    QSqlDatabase db = QSqlDatabase::database("learnPascal");
    if (db.isOpen())
    {
        QString q = "SELECT tasks FROM users "
                    "WHERE login= \'" + this->profile.name +"\';";
        QSqlQuery query(db);
        if (!query.exec(q))
            qDebug() << q << "\n" << query.lastError().text();
        else
        {
            query.next();
            this->profile.tasks = query.value(0).toString();
            loadTaskTree();
        }
    }
}

void MainWindow::on_action_exitProfile_triggered()
{
    init();
}

void MainWindow::on_action_manager_triggered()
{
    if (profile.administrator)
    {
        TasksDialog manager;
        manager.setTaskTree(ui->treeWidget);
        manager.exec();
        loadProfileTasks();
        loadTaskTree();
    }
    else
        msgBoxSimple("Ошибка", "У вас нет прав администратора");
}

void MainWindow::highlightString(int n)
{
    QTextCursor qtc = QTextCursor(ui->programEdit->document());
    qtc.movePosition(QTextCursor::Start);
    qtc.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, n-1);
    qtc.select(QTextCursor::LineUnderCursor);
    QTextCharFormat format;
    format.setBackground(Qt::red);
    QTextEdit::ExtraSelection selec;
    selec.cursor = qtc;
    selec.format = format;
    ui->programEdit->setExtraSelections(QList<QTextEdit::ExtraSelection>() << selec);
    qtc.movePosition(QTextCursor::StartOfLine);
    ui->programEdit->setTextCursor(qtc);
    hightlighted = true;
}

void MainWindow::on_programEdit_textChanged()
{
    if (hightlighted)
    {
        auto backup = ui->programEdit->textCursor();
        QTextCursor qtc = QTextCursor(ui->programEdit->document());
        qtc.select(QTextCursor::Document);
        QTextCharFormat format;
        format.setBackground(Qt::transparent);
        QTextEdit::ExtraSelection selec;
        selec.cursor = qtc;
        selec.format = format;
        ui->programEdit->setExtraSelections(QList<QTextEdit::ExtraSelection>() << selec);
        ui->programEdit->setTextCursor(backup);
        hightlighted = false;
    }
}

void MainWindow::on_action_about_triggered()
{
    AboutDialog dialog;
    dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    dialog.exec();
}

void MainWindow::on_action_help_triggered()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::currentPath() + "/help.chm"));
}
