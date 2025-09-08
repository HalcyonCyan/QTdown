/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QSpacerItem *verticalSpacer_2;
    QPushButton *startButton;
    QPushButton *restartButton;
    QPushButton *settingsButton;
    QPushButton *exitButton;
    QSpacerItem *verticalSpacer;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(400, 500);
        MainWindow->setStyleSheet(QString::fromUtf8("QMainWindow {\n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,\n"
"                                    stop:0 #6a3093, stop:1 #a044ff);\n"
"}\n"
"\n"
"QLabel {\n"
"    color: white;\n"
"    background: transparent;\n"
"}\n"
"\n"
"QPushButton {\n"
"    background-color: rgba(255, 255, 255, 200);\n"
"    border: 2px solid #8A2BE2;\n"
"    border-radius: 10px;\n"
"    color: #4B0082;\n"
"    font-weight: bold;\n"
"    padding: 10px;\n"
"    min-width: 150px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgba(255, 255, 255, 230);\n"
"    border: 2px solid #9370DB;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: rgba(200, 200, 255, 200);\n"
"}"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        titleLabel = new QLabel(centralwidget);
        titleLabel->setObjectName("titleLabel");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(titleLabel->sizePolicy().hasHeightForWidth());
        titleLabel->setSizePolicy(sizePolicy);
        QFont font;
        font.setPointSize(24);
        font.setBold(true);
        titleLabel->setFont(font);
        titleLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(titleLabel);

        subtitleLabel = new QLabel(centralwidget);
        subtitleLabel->setObjectName("subtitleLabel");
        QFont font1;
        font1.setPointSize(12);
        subtitleLabel->setFont(font1);
        subtitleLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(subtitleLabel);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        startButton = new QPushButton(centralwidget);
        startButton->setObjectName("startButton");
        QFont font2;
        font2.setPointSize(14);
        startButton->setFont(font2);

        verticalLayout->addWidget(startButton);

        restartButton = new QPushButton(centralwidget);
        restartButton->setObjectName("restartButton");
        restartButton->setFont(font2);

        verticalLayout->addWidget(restartButton);

        settingsButton = new QPushButton(centralwidget);
        settingsButton->setObjectName("settingsButton");
        settingsButton->setFont(font2);

        verticalLayout->addWidget(settingsButton);

        exitButton = new QPushButton(centralwidget);
        exitButton->setObjectName("exitButton");
        exitButton->setFont(font2);

        verticalLayout->addWidget(exitButton);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "DeepRock - Platform Jumping Game", nullptr));
        titleLabel->setText(QCoreApplication::translate("MainWindow", "DeepRock", nullptr));
        subtitleLabel->setText(QCoreApplication::translate("MainWindow", "Platform Jumping Adventure", nullptr));
        startButton->setText(QCoreApplication::translate("MainWindow", "Start Game", nullptr));
        restartButton->setText(QCoreApplication::translate("MainWindow", "Restart", nullptr));
        settingsButton->setText(QCoreApplication::translate("MainWindow", "Settings", nullptr));
        exitButton->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
