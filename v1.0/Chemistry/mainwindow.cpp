#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Equation.h"
#include <QValidator>
#include <QTime>
#include <QWizard>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //配置界面
    ui->setupUi(this);
    ui->statusBar->showMessage("欢迎使用化学反应配平器", 4000);
    QLabel* annoucement=new QLabel(this);
    annoucement->setText("DYZ(C) Gamma 2024.1.19");
    ui->statusBar->addPermanentWidget(annoucement);
    //设置过滤器
    QValidator* validator = new QRegularExpressionValidator(QRegularExpression("([A-Z0-9a-z·\\-\\+\\[\\]\\(\\)<> .—]*)*"), this);
    ui->lineEdit->setValidator(validator);
}

MainWindow::~MainWindow()
{
    delete ui;
}

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
    wizard.setWindowTitle("使用辅助");
    wizard.setMinimumSize(600, 45);
    wizard.addPage(createPage("合法的物质输入", "Na[Al(OH)4] [CuCl2·CuCO3] e<-> NH4<+> \n支持括号嵌套", &wizard));
    wizard.addPage(createPage("输入示范", "Fe(CN)6<4-> +MnO4<->+188H<+>-- Mn<2+>+Fe<3+>+CO2+NO3<->+94H2O"
        "\n(空格随便)", &wizard));
    wizard.addPage(createPage("反应条件", "支持 —— -- 中间可插入 32K -12C 自动换算", &wizard));
    wizard.addPage(createPage("输出说明", "错误提示与位置\n配平结果(唯一解) 拆分结果(无穷解)\n"
        "线性组合数众多，结果很玄学，改变待定系数|输入顺序都能影响", &wizard));
    wizard.exec();
}

void MainWindow::on_pushButton_clicked()//点击按钮
{
    ui->lineEdit->setText(ui->lineEdit->text().simplified().remove(' '));
    ui->plainTextEdit->setPlainText("");
    ui->plainTextEdit->appendPlainText(QTime::currentTime().toString("[hh:mm:ss]"));
    try
    {
        Equation mine(ui->lineEdit->text());
        mine.balance();
        ui->plainTextEdit->appendHtml(mine.print());
    }
    catch (QString error) { ui->plainTextEdit->appendPlainText(error); }
}
