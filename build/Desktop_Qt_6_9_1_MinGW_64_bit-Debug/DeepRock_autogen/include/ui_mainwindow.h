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
    QLabel *accountLabel;
    QSpacerItem *verticalSpacer_2;
    QPushButton *singlePlayerButton;
    QPushButton *multiplayerButton;
    QPushButton *settingsButton;
    QPushButton *leaderboardButton;
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

        accountLabel = new QLabel(centralwidget);
        accountLabel->setObjectName("accountLabel");
        QFont font2;
        font2.setPointSize(10);
        accountLabel->setFont(font2);
        accountLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(accountLabel);

        verticalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        singlePlayerButton = new QPushButton(centralwidget);
        singlePlayerButton->setObjectName("singlePlayerButton");
        QFont font3;
        font3.setPointSize(14);
        singlePlayerButton->setFont(font3);

        verticalLayout->addWidget(singlePlayerButton);

        multiplayerButton = new QPushButton(centralwidget);
        multiplayerButton->setObjectName("multiplayerButton");
        multiplayerButton->setFont(font3);

        verticalLayout->addWidget(multiplayerButton);

        settingsButton = new QPushButton(centralwidget);
        settingsButton->setObjectName("settingsButton");
        settingsButton->setFont(font3);

        verticalLayout->addWidget(settingsButton);

        leaderboardButton = new QPushButton(centralwidget);
        leaderboardButton->setObjectName("leaderboardButton");
        leaderboardButton->setFont(font3);

        verticalLayout->addWidget(leaderboardButton);

        exitButton = new QPushButton(centralwidget);
        exitButton->setObjectName("exitButton");
        exitButton->setFont(font3);

        verticalLayout->addWidget(exitButton);

        verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

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
        accountLabel->setText(QCoreApplication::translate("MainWindow", "\350\264\246\345\217\267: Player\\n\346\234\200\351\253\230\345\210\206: 0", nullptr));
        singlePlayerButton->setText(QCoreApplication::translate("MainWindow", "Single-player Game", nullptr));
        multiplayerButton->setText(QCoreApplication::translate("MainWindow", "Multiplayer Game", nullptr));
        settingsButton->setText(QCoreApplication::translate("MainWindow", "Settings", nullptr));
        leaderboardButton->setText(QCoreApplication::translate("MainWindow", "Leaderboard", nullptr));
        exitButton->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
