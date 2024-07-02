#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTime>
#include <QDebug>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //配置界面
    ui->setupUi(this);
    ui->statusBar->showMessage(QStringLiteral("欢迎使用热化学反应解析器"), 8000);
    ui->statusBar->addPermanentWidget(new QLabel("DYZ(C) 2024.7.2 4.0", this));
    //连接core
    core->setProgram("Core.exe");
    connect(core, &QProcess::errorOccurred, this, &MainWindow::error);
    connect(core, &QProcess::readyReadStandardOutput, this, &MainWindow::output);
    core->start(QIODevice::ReadWrite);
}

MainWindow::~MainWindow() { core->write("[exit]\n"); delete core; delete ui; }

void MainWindow::on_action_H_triggered()//使用辅助
{
    ui->plainTextEdit->setPlainText(QStringLiteral("|使用辅助"));
    ui->plainTextEdit->appendPlainText(QStringLiteral("合法物质:Na[Al(OH)4] e<-> NH4<+>(aq) 1/2O2 支持括号嵌套"));
    ui->plainTextEdit->appendPlainText(QStringLiteral("输入示范:1/3H2O(l)+Cl2(g)--H<+>(aq)+Cl<->(aq)+HClO(aq) 空格随便"));
    ui->plainTextEdit->appendPlainText(QStringLiteral("条件示范:暂时无限制"));
    ui->plainTextEdit->appendPlainText(QStringLiteral("输出说明:错误提示 或 解析结果"));
}

void MainWindow::on_pushButton_clicked()//点击按钮
{
    ui->plainTextEdit->clearMask();
    ui->plainTextEdit->setPlainText(QTime::currentTime().toString("|hh:mm:ss"));
    ui->lineEdit->setText(ui->lineEdit->text().trimmed().remove(" "));
    ui->lineEdit2->setText(ui->lineEdit2->text().trimmed().remove(" "));
    if (ui->lineEdit2->text().isEmpty())ui->lineEdit2->setText("<none>");
    if (ui->lineEdit->text().isEmpty() || ui->lineEdit->text() == "[exit]")
    {
        ui->plainTextEdit->appendPlainText(QStringLiteral("化学式错误"));
        return;
    }
    core->write((ui->lineEdit->text() + "\n").toLocal8Bit());
    core->write((ui->lineEdit2->text() + "\n").toLocal8Bit());
}

void MainWindow::error()
{
    ui->plainTextEdit->appendPlainText(QStringLiteral("Core进程错误:"));
    ui->plainTextEdit->appendPlainText(core->errorString());
    ui->pushButton->setDisabled(true);
}

void MainWindow::output()
{
    QByteArray arr = core->readAllStandardOutput();
    static QStringDecoder toUtf = QStringDecoder(QStringDecoder::System);
    ui->plainTextEdit->appendHtml(toUtf(arr));
}