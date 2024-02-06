/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_github;
    QAction *action_bug;
    QAction *action_H;
    QAction *action_D;
    QAction *action_Qt;
    QAction *action_this;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QLineEdit *lineEdit;
    QPlainTextEdit *plainTextEdit;
    QPushButton *pushButton;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menu_H;
    QMenu *menu_R;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1000, 650);
        MainWindow->setMinimumSize(QSize(1000, 650));
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(12);
        MainWindow->setFont(font);
        action_github = new QAction(MainWindow);
        action_github->setObjectName(QString::fromUtf8("action_github"));
        action_github->setCheckable(false);
        action_bug = new QAction(MainWindow);
        action_bug->setObjectName(QString::fromUtf8("action_bug"));
        action_H = new QAction(MainWindow);
        action_H->setObjectName(QString::fromUtf8("action_H"));
        action_D = new QAction(MainWindow);
        action_D->setObjectName(QString::fromUtf8("action_D"));
        action_Qt = new QAction(MainWindow);
        action_Qt->setObjectName(QString::fromUtf8("action_Qt"));
        action_this = new QAction(MainWindow);
        action_this->setObjectName(QString::fromUtf8("action_this"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        lineEdit = new QLineEdit(centralwidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setMinimumSize(QSize(0, 40));
        lineEdit->setFont(font);
#ifndef QT_NO_TOOLTIP
        lineEdit->setToolTip(QString::fromUtf8(""));
#endif // QT_NO_TOOLTIP

        verticalLayout->addWidget(lineEdit);

        plainTextEdit = new QPlainTextEdit(centralwidget);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setEnabled(true);
        plainTextEdit->setFont(font);
#ifndef QT_NO_TOOLTIP
        plainTextEdit->setToolTip(QString::fromUtf8(""));
#endif // QT_NO_TOOLTIP
        plainTextEdit->setReadOnly(true);

        verticalLayout->addWidget(plainTextEdit);

        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setMinimumSize(QSize(0, 40));
        pushButton->setFont(font);
        pushButton->setCursor(QCursor(Qt::PointingHandCursor));

        verticalLayout->addWidget(pushButton);

        MainWindow->setCentralWidget(centralwidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1000, 19));
        menu_H = new QMenu(menuBar);
        menu_H->setObjectName(QString::fromUtf8("menu_H"));
        menu_R = new QMenu(menuBar);
        menu_R->setObjectName(QString::fromUtf8("menu_R"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menu_H->menuAction());
        menuBar->addAction(menu_R->menuAction());
        menu_H->addAction(action_H);
        menu_H->addSeparator();
        menu_H->addAction(action_D);
        menu_H->addAction(action_Qt);
        menu_H->addAction(action_this);
        menu_R->addAction(action_github);
        menu_R->addAction(action_bug);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "\345\214\226\345\255\246\345\217\215\345\272\224\350\247\243\346\236\220\345\231\250", nullptr));
        action_github->setText(QApplication::translate("MainWindow", "\346\272\220\344\273\243\347\240\201\350\216\267\345\217\226", nullptr));
        action_bug->setText(QApplication::translate("MainWindow", "\346\212\245\345\221\212bug", nullptr));
        action_H->setText(QApplication::translate("MainWindow", "\344\275\277\347\224\250\350\276\205\345\212\251", nullptr));
        action_D->setText(QApplication::translate("MainWindow", "\345\205\263\344\272\216\345\274\200\345\217\221", nullptr));
        action_Qt->setText(QApplication::translate("MainWindow", "\345\205\263\344\272\216Qt", nullptr));
        action_this->setText(QApplication::translate("MainWindow", "\345\205\263\344\272\216\350\247\243\346\236\220\345\231\250", nullptr));
        lineEdit->setPlaceholderText(QApplication::translate("MainWindow", "H<+>+OH<->--H2O", nullptr));
        plainTextEdit->setPlaceholderText(QApplication::translate("MainWindow", "\350\276\223\345\207\272\347\273\223\346\236\234", nullptr));
        pushButton->setText(QApplication::translate("MainWindow", "\350\247\243\346\236\220", nullptr));
        menu_H->setTitle(QApplication::translate("MainWindow", "\345\205\263\344\272\216(&H)", nullptr));
        menu_R->setTitle(QApplication::translate("MainWindow", "\345\217\215\351\246\210(&R)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
