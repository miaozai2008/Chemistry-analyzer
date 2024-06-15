#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "equation.h"
#include <QTime>
#include <QWizard>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //配置界面
    ui->setupUi(this);
    ui->statusBar->showMessage(QStringLiteral("欢迎使用热化学反应解析器"), 8000);
    ui->statusBar->addPermanentWidget(new QLabel("DYZ(C) 2024.6.8 3.0", this));
}

MainWindow::~MainWindow() { delete ui; }

QWizardPage* createPage(const QString& title, const QString& text, QWidget* parent = nullptr)//创建导航页
{
    QWizardPage* p = new QWizardPage(parent);
    p->setTitle(title);
    QVBoxLayout* v = new QVBoxLayout(p);
    QLabel* l = new QLabel(p);
    l->setText(text);
    l->setWordWrap(true);
    v->addWidget(l);
    p->setLayout(v);
    return p;
}

void MainWindow::on_action_H_triggered()//使用辅助
{
    QWizard wizard(this);
    wizard.setWindowTitle(QStringLiteral("使用辅助"));
    wizard.setMinimumSize(600, 45);
    wizard.addPage(createPage(QStringLiteral("合法的物质输入"), 
        QStringLiteral("Na[Al(OH)4] e<-> NH4<+> 1/2O2 \n支持括号嵌套"), &wizard));
    wizard.addPage(createPage(QStringLiteral("输入示范"),
        QStringLiteral("Fe(CN)6<4-> +MnO4<->+188/3H<+>-- Mn<2+>+Fe<3+>+CO2+NO3<->+94/3H2O\n(空格随便)"), &wizard));
    wizard.addPage(createPage(QStringLiteral("输出说明"),
        QStringLiteral("错误提示与位置\n配平结果(唯一解) 拆分结果(无穷解)"), &wizard));
    wizard.exec();
}

void MainWindow::on_pushButton_clicked()//点击按钮
{
    ui->lineEdit->setText(ui->lineEdit->text().simplified().remove(' '));
    ui->lineEdit2->setText(ui->lineEdit2->text().simplified());
    ui->plainTextEdit->clearMask();
    ui->plainTextEdit->setPlainText("");
    ui->plainTextEdit->appendPlainText(QTime::currentTime().toString("[hh:mm:ss]"));
    try
    {
        equation mine(ui->lineEdit->text(), ui->lineEdit2->text());
        mine.balance();
        ui->plainTextEdit->appendHtml(mine.print());
    }
    catch (const wstring& error) { ui->plainTextEdit->appendPlainText(QString::fromStdWString(error)); }
}