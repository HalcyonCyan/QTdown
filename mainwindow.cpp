#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QInputDialog>
#include <QDebug>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include "leaderboard.h"
#include "leaderboardwidget.h"
#include "network.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , gameWindow(nullptr)
    , gameRunning(false)
{
    ui->setupUi(this);

    
    setWindowTitle(tr("DeepRock - Platform Jumping Game"));
    setFixedSize(400, 500);

    
    if (!currentAccount.loadAccount("default")) {

        currentAccount.createAccount("Player", "", "default");
    }

    updateAccountDisplay();
}

MainWindow::~MainWindow()
{
    delete ui;
    if (gameWindow) delete gameWindow;
}

void MainWindow::updateAccountDisplay()
{
    ui->accountLabel->setText(
        QString(tr("账号: %1\n最高分: %2"))
            .arg(currentAccount.getUsername())
            .arg(currentAccount.getHighScore())
        );
}

void MainWindow::showAccountDialog()
{
    
    QDialog dialog(this);
    dialog.setWindowTitle(tr("账号管理"));
    dialog.setFixedSize(400, 300);  

    QVBoxLayout layout(&dialog);
    layout.setSpacing(15);

    
    QLabel infoLabel(&dialog);
    infoLabel.setText(
        QString(tr("当前账号: %1\n账号ID: %2\n最高分: %3"))
            .arg(currentAccount.getUsername())
            .arg(currentAccount.getGameId())
            .arg(currentAccount.getHighScore())
        );
    infoLabel.setWordWrap(true);
    infoLabel.setMinimumHeight(120); 
    infoLabel.setAlignment(Qt::AlignLeft | Qt::AlignTop); 
    infoLabel.setStyleSheet("font-size: 12px;"); 
    layout.addWidget(&infoLabel);

    
    QPushButton registerButton(tr("注册账号"), &dialog);
    QPushButton changeButton(tr("更换账号"), &dialog);
    QPushButton closeButton(tr("关闭"), &dialog);

    
    registerButton.setFixedSize(140, 40);
    changeButton.setFixedSize(140, 40);
    closeButton.setFixedSize(140, 40);

    
    QString buttonStyle = "QPushButton {"
                          "font-size: 14px;"
                          "font-weight: bold;"
                          "border-radius: 5px;"
                          "background-color: #f0f0f0;"
                          "border: 1px solid #ccc;"
                          "}"
                          "QPushButton:hover {"
                          "background-color: #e0e0e0;"
                          "}"
                          "QPushButton:pressed {"
                          "background-color: #d0d0d0;"
                          "}";

    registerButton.setStyleSheet(buttonStyle);
    changeButton.setStyleSheet(buttonStyle);
    closeButton.setStyleSheet(buttonStyle);

    
    QVBoxLayout buttonLayout;
    buttonLayout.setSpacing(15); 
    buttonLayout.addStretch(); 
    buttonLayout.addWidget(&registerButton, 0, Qt::AlignCenter); 
    buttonLayout.addWidget(&changeButton, 0, Qt::AlignCenter); 
    buttonLayout.addWidget(&closeButton, 0, Qt::AlignCenter); 
    buttonLayout.addStretch(); 

    layout.addLayout(&buttonLayout);

    
    connect(&registerButton, &QPushButton::clicked, [this, &dialog]() {
        dialog.close();
        this->registerAccount();
    });

    connect(&changeButton, &QPushButton::clicked, [this, &dialog]() {
        dialog.close();
        this->changeAccount();
    });

    connect(&closeButton, &QPushButton::clicked, [&dialog]() {
        dialog.close();
    });

    
    dialog.setStyleSheet("QLabel { color: black; font-size: 12px; } QPushButton { color: black; }");

    dialog.exec();
}

void MainWindow::registerAccount()
{
    
    QDialog dialog(this);
    dialog.setWindowTitle(tr("注册账号"));
    dialog.setFixedSize(400, 320);  

    QFormLayout form(&dialog);
    form.setSpacing(15); 
    form.setLabelAlignment(Qt::AlignRight); 

    
    QLineEdit usernameEdit;
    usernameEdit.setPlaceholderText(tr("请输入用户名"));
    usernameEdit.setMinimumHeight(30);
    form.addRow(tr("用户名:"), &usernameEdit);

    
    QLineEdit passwordEdit;
    passwordEdit.setPlaceholderText(tr("请输入密码"));
    passwordEdit.setEchoMode(QLineEdit::Password);
    passwordEdit.setMinimumHeight(30);
    form.addRow(tr("密码:"), &passwordEdit);

    
    QLineEdit confirmPasswordEdit;
    confirmPasswordEdit.setPlaceholderText(tr("请确认密码"));
    confirmPasswordEdit.setEchoMode(QLineEdit::Password);
    confirmPasswordEdit.setMinimumHeight(30);
    form.addRow(tr("确认密码:"), &confirmPasswordEdit);

    
    QHBoxLayout buttonLayout;
    buttonLayout.setSpacing(20);

    QPushButton okButton(tr("确定"), &dialog);
    QPushButton cancelButton(tr("取消"), &dialog);

    
    okButton.setFixedSize(120, 40);
    cancelButton.setFixedSize(120, 40);

    QString buttonStyle = "QPushButton {"
                          "font-size: 14px;"
                          "font-weight: bold;"
                          "border-radius: 5px;"
                          "background-color: #f0f0f0;"
                          "border: 1px solid #ccc;"
                          "}"
                          "QPushButton:hover {"
                          "background-color: #e0e0e0;"
                          "}"
                          "QPushButton:pressed {"
                          "background-color: #d0d0d0;"
                          "}";

    okButton.setStyleSheet(buttonStyle);
    cancelButton.setStyleSheet(buttonStyle);

    buttonLayout.addStretch(); 
    buttonLayout.addWidget(&okButton);
    buttonLayout.addWidget(&cancelButton);
    buttonLayout.addStretch(); 

    form.addRow("", new QWidget); 
    form.addRow("", &buttonLayout); 

    
    connect(&okButton, &QPushButton::clicked, [&]() {
        QString username = usernameEdit.text().trimmed();
        QString password = passwordEdit.text();
        QString confirmPassword = confirmPasswordEdit.text();

        if (username.isEmpty()) {
            QMessageBox::warning(&dialog, tr("错误"), tr("用户名不能为空"));
            return;
        }

        if (password.isEmpty()) {
            QMessageBox::warning(&dialog, tr("错误"), tr("密码不能为空"));
            return;
        }

        if (password != confirmPassword) {
            QMessageBox::warning(&dialog, tr("错误"), tr("两次输入的密码不一致"));
            return;
        }

        
        Account newAccount;
        if (newAccount.createAccount(username, password, "default")) {
            currentAccount = newAccount;
            updateAccountDisplay();
            QMessageBox::information(&dialog, tr("成功"),
                                     tr("账号注册成功!\n您的账号ID: %1\n请牢记您的账号ID和密码").arg(newAccount.getGameId()));
            dialog.accept();
        } else {
            QMessageBox::warning(&dialog, tr("错误"),
                                 tr("账号创建失败，可能是账号ID已存在或文件权限问题"));
        }
    });

    connect(&cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    
    dialog.setStyleSheet("QLabel { color: black; font-size: 12px; } QLineEdit { color: black; font-size: 12px; } QPushButton { color: black; }");

    dialog.exec();
}


void MainWindow::onHostCreated(const QString &ip, quint16 port)
{
    
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setWindowTitle(tr("主机创建成功"));
    msgBox->setText(tr("主机已创建成功！\nIP地址: %1\n端口: %2\n请将此信息分享给其他玩家").arg(ip).arg(port));
    msgBox->setStyleSheet("QLabel{ color: black; }");
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setModal(false); 
    msgBox->show();

    
    connect(msgBox, &QMessageBox::finished, msgBox, &QMessageBox::deleteLater);
}




void MainWindow::changeAccount()
{
    
    QDialog dialog(this);
    dialog.setWindowTitle(tr("更换账号"));
    dialog.setFixedSize(400, 220);  

    QFormLayout form(&dialog);
    form.setSpacing(15); 
    form.setLabelAlignment(Qt::AlignRight); 

    
    QLineEdit accountIdEdit;
    accountIdEdit.setPlaceholderText(tr("请输入账号ID"));
    accountIdEdit.setMinimumHeight(30);
    form.addRow(tr("账号ID:"), &accountIdEdit);

    
    QLineEdit passwordEdit;
    passwordEdit.setPlaceholderText(tr("请输入密码"));
    passwordEdit.setEchoMode(QLineEdit::Password);
    passwordEdit.setMinimumHeight(30);
    form.addRow(tr("密码"), &passwordEdit);

    
    QHBoxLayout buttonLayout;
    buttonLayout.setSpacing(20); 

    QPushButton okButton(tr("确定"), &dialog);
    QPushButton cancelButton(tr("取消"), &dialog);

    
    okButton.setFixedSize(120, 40);
    cancelButton.setFixedSize(120, 40);

    QString buttonStyle = "QPushButton {"
                          "font-size: 14px;"
                          "font-weight: bold;"
                          "border-radius: 5px;"
                          "background-color: #f0f0f0;"
                          "border: 1px solid #ccc;"
                          "}"
                          "QPushButton:hover {"
                          "background-color: #e0e0e0;"
                          "}"
                          "QPushButton:pressed {"
                          "background-color: #d0d0d0;"
                          "}";

    okButton.setStyleSheet(buttonStyle);
    cancelButton.setStyleSheet(buttonStyle);

    buttonLayout.addStretch(); 
    buttonLayout.addWidget(&okButton);
    buttonLayout.addWidget(&cancelButton);
    buttonLayout.addStretch(); 

    form.addRow("", new QWidget); 
    form.addRow("", &buttonLayout); 

    
    connect(&okButton, &QPushButton::clicked, [&]() {
        QString accountId = accountIdEdit.text().trimmed();
        QString password = passwordEdit.text();

        if (accountId.isEmpty()) {
            QMessageBox::warning(&dialog, tr("错误"), tr("账号ID不能为空"));
            return;
        }

        Account newAccount;
        if (newAccount.loadAccount(accountId)) {
            if (newAccount.checkPassword(password)) {
                currentAccount = newAccount;
                updateAccountDisplay();
                QMessageBox::information(&dialog, tr("成功"), tr("账号切换成功"));
                dialog.accept();
            } else {
                QMessageBox::warning(&dialog, tr("错误"), tr("密码错误"));
            }
        } else {
            QMessageBox::warning(&dialog, tr("错误"), tr("账号不存在"));
        }
    });

    connect(&cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    
    dialog.setStyleSheet("QLabel { color: black; font-size: 12px; } QLineEdit { color: black; font-size: 12px; } QPushButton { color: black; }");

    dialog.exec();
}

void GameWindow::startGame()
{
    gameEngine->initGame();

    
    if (isMultiplayerMode && network && isHost && network->isConnected()) {
        network->requestGameStart(); 
    }

    timer->start(8);
    gamePaused = false;
}


void MainWindow::showMultiplayerDialog()
{
    
    QStringList items;
    items << tr("创建主机") << tr("连接主机");

    bool ok;

    
    QInputDialog dialog(this);
    dialog.setWindowTitle(tr("多人游戏"));
    dialog.setLabelText(tr("选择多人游戏模式:"));
    dialog.setComboBoxItems(items);
    dialog.setComboBoxEditable(false);
    dialog.setStyleSheet("color: black;"); 

    ok = dialog.exec();
    QString choice = dialog.textValue();

    if (ok && !choice.isEmpty()) {
        if (choice == tr("创建主机")) {
            
            QInputDialog portDialog(this);
            portDialog.setWindowTitle(tr("创建主机"));
            portDialog.setLabelText(tr("请输入端口号:"));
            portDialog.setIntValue(12345);
            portDialog.setIntMinimum(1024);
            portDialog.setIntMaximum(65535);
            portDialog.setIntStep(1);
            portDialog.setStyleSheet("color: black;"); 

            bool portOk = portDialog.exec();
            int port = portDialog.intValue();

            if (portOk) {
                
                Network* network = new Network(this);
                network->setCurrentAccount(currentAccount);

                
                connect(network, &Network::hostCreated, this, &MainWindow::onHostCreated);

                
                waitingDialog = new WaitingDialog(network, true, this);
                connect(waitingDialog, &WaitingDialog::gameStarted, this, [this, network]() {
                    
                    startMultiplayerGame(network, true);
                    waitingDialog->deleteLater();
                    waitingDialog = nullptr;
                });
                connect(waitingDialog, &WaitingDialog::cancelled, this, [this, network]() {
                    network->deleteLater();
                    waitingDialog->deleteLater();
                    waitingDialog = nullptr;
                });

                if (network->createHost(port)) {
                    waitingDialog->exec();
                } else {
                    QMessageBox::warning(this, tr("错误"),
                                         tr("无法创建主机: %1").arg(network->errorString()));
                    
                    network->disconnect();
                    delete network;
                }
            }
        } else {
            
            QInputDialog ipDialog(this);
            ipDialog.setWindowTitle(tr("连接主机"));
            ipDialog.setLabelText(tr("请输入主机IP地址:"));
            ipDialog.setTextValue("127.0.0.1");
            ipDialog.setStyleSheet("color: black;"); 

            bool ipOk = ipDialog.exec();
            QString ip = ipDialog.textValue();

            if (ipOk && !ip.isEmpty()) {
                QInputDialog portDialog(this);
                portDialog.setWindowTitle(tr("连接主机"));
                portDialog.setLabelText(tr("请输入端口号:"));
                portDialog.setIntValue(12345);
                portDialog.setIntMinimum(1024);
                portDialog.setIntMaximum(65535);
                portDialog.setIntStep(1);
                portDialog.setStyleSheet("color: black;"); 

                bool portOk = portDialog.exec();
                int port = portDialog.intValue();

                if (portOk) {
                    
                    Network* network = new Network(this);
                    network->setCurrentAccount(currentAccount);

                    
                    waitingDialog = new WaitingDialog(network, false, this);
                    connect(waitingDialog, &WaitingDialog::gameStarted, this, [this, network]() {
                        
                        startMultiplayerGame(network, false);
                        waitingDialog->deleteLater();
                        waitingDialog = nullptr;
                    });
                    connect(waitingDialog, &WaitingDialog::cancelled, this, [this, network]() {
                        network->deleteLater();
                        waitingDialog->deleteLater();
                        waitingDialog = nullptr;
                    });

                    if (network->connectToHost(ip, port)) {
                        waitingDialog->exec();
                    } else {
                        QMessageBox::warning(this, tr("错误"),
                                             tr("无法连接到主机: %1").arg(network->errorString()));
                        delete network;
                    }
                }
            }
        }
    }
}


void MainWindow::startMultiplayerGame(Network* network, bool isHost)
{
    if (!gameWindow) {
        
        gameWindow = new GameWindow(true, network, nullptr);
        connect(gameWindow, &GameWindow::gameFinished, this, &MainWindow::gameFinished);
    } else {
        
        gameWindow->setNetwork(network, isHost);
    }

    gameWindow->startGame();
    gameWindow->show();
    this->hide();
    gameRunning = true;
}
void MainWindow::showLeaderboard()
{
    Leaderboard *lb = Leaderboard::instance();
    QList<Account> topScores = lb->getTopScores(50); 

    
    QDialog dialog(this);
    dialog.setWindowTitle(tr("排行榜"));
    dialog.setFixedSize(500, 500);

    QVBoxLayout layout(&dialog);

    
    QTableWidget table(topScores.size(), 4, &dialog);
    table.setHorizontalHeaderLabels(QStringList() << tr("排名") << tr("账号ID") << tr("用户名") << tr("分数"));
    table.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table.setEditTriggers(QAbstractItemView::NoEditTriggers);
    table.setSelectionBehavior(QAbstractItemView::SelectRows);

    
    for (int i = 0; i < topScores.size(); i++) {
        const Account &acc = topScores[i];

        QTableWidgetItem *rankItem  = new QTableWidgetItem(QString::number(i + 1));
        QTableWidgetItem *idItem    = new QTableWidgetItem(acc.getGameId());
        QTableWidgetItem *nameItem  = new QTableWidgetItem(acc.getUsername());
        QTableWidgetItem *scoreItem = new QTableWidgetItem(QString::number(acc.getHighScore()));

        rankItem->setTextAlignment(Qt::AlignCenter);
        idItem->setTextAlignment(Qt::AlignCenter);
        nameItem->setTextAlignment(Qt::AlignCenter);
        scoreItem->setTextAlignment(Qt::AlignCenter);

        table.setItem(i, 0, rankItem);
        table.setItem(i, 1, idItem);
        table.setItem(i, 2, nameItem);
        table.setItem(i, 3, scoreItem);
    }

    layout.addWidget(&table);

    
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton clearButton(tr("清空排行榜"), &dialog);
    QPushButton closeButton(tr("关闭"), &dialog);

    buttonLayout->addWidget(&clearButton);
    buttonLayout->addWidget(&closeButton);

    layout.addLayout(buttonLayout);

    
    connect(&closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(&clearButton, &QPushButton::clicked, [&]() {
        lb->clear();
        QMessageBox::information(&dialog, tr("提示"), tr("排行榜已清空！"));
        dialog.accept(); 
    });

    dialog.exec();
}




void MainWindow::on_singlePlayerButton_clicked()
{
    qDebug() << "Single-player button clicked";

    if (!gameWindow) {
        qDebug() << "Creating new GameWindow";
        gameWindow = new GameWindow(false, nullptr); 
        connect(gameWindow, &GameWindow::gameFinished, this, &MainWindow::gameFinished);
    }

    qDebug() << "Starting single-player game";
    gameWindow->startGame();
    qDebug() << "Showing game window";
    gameWindow->show();
    qDebug() << "Hiding main window";
    this->hide();
    gameRunning = true;
}

void MainWindow::on_multiplayerButton_clicked()
{
    showMultiplayerDialog();
}

void MainWindow::on_settingsButton_clicked()
{
    
    QStringList items;
    items << tr("账号管理") << tr("游戏设置");

    
    QInputDialog dialog(this);
    dialog.setWindowTitle(tr("设置"));
    dialog.setLabelText(tr("选择设置选项:"));
    dialog.setComboBoxItems(items);
    dialog.setComboBoxEditable(false);
    dialog.setStyleSheet("color: black;"); 

    bool ok = dialog.exec();
    QString choice = dialog.textValue();

    if (ok && !choice.isEmpty()) {
        if (choice == tr("账号管理")) {
            showAccountDialog();
        } else {
            
            QStringList difficultyOptions;
            difficultyOptions << tr("简单") << tr("中等") << tr("困难");

            QInputDialog difficultyDialog(this);
            difficultyDialog.setWindowTitle(tr("游戏设置"));
            difficultyDialog.setLabelText(tr("选择游戏难度:"));
            difficultyDialog.setComboBoxItems(difficultyOptions);
            difficultyDialog.setComboBoxEditable(false);
            difficultyDialog.setStyleSheet("color: black;"); 

            bool difficultyOk = difficultyDialog.exec();
            QString difficulty = difficultyDialog.textValue();

            if (difficultyOk && !difficulty.isEmpty()) {
                
                if (difficulty == tr("简单")) {
                    QMessageBox msgBox;
                    msgBox.setWindowTitle(tr("游戏设置"));
                    msgBox.setText(tr("已设置为简单难度\n"));
                    msgBox.setStyleSheet("QLabel { color: black; }");
                    msgBox.exec();
                } else if (difficulty == tr("中等")) {
                    QMessageBox msgBox;
                    msgBox.setWindowTitle(tr("游戏设置"));
                    msgBox.setText(tr("已设置为中等难度\n"));
                    msgBox.setStyleSheet("QLabel { color: black; }");
                    msgBox.exec();
                } else if (difficulty == tr("困难")) {
                    QMessageBox msgBox;
                    msgBox.setWindowTitle(tr("游戏设置"));
                    msgBox.setText(tr("已设置为困难难度\n"));
                    msgBox.setStyleSheet("QLabel { color: black; }");
                    msgBox.exec();
                }

            }
        }
    }
}

void MainWindow::on_leaderboardButton_clicked()
{
    showLeaderboard();
}

void MainWindow::on_exitButton_clicked()
{
    QApplication::quit();
}

void MainWindow::gameFinished()
{
    this->show();
    gameRunning = false;

    
    if (gameWindow && gameWindow->getScore() > currentAccount.getHighScore()) {
        currentAccount.setHighScore(gameWindow->getScore());
        updateAccountDisplay();

        
        Leaderboard *lb = Leaderboard::instance();
        lb->updateAccount(currentAccount);
        lb->save();
    }

    
    showLeaderboard();
}



