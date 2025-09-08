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

    // 设置窗口标题和大小
    setWindowTitle(tr("DeepRock - Platform Jumping Game"));
    setFixedSize(400, 500);

    // 尝试加载默认账号
    if (!currentAccount.loadAccount("default")) {
        // 如果没有默认账号，创建一个默认账号
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
    // 创建账号管理对话框 - 增加高度
    QDialog dialog(this);
    dialog.setWindowTitle(tr("账号管理"));
    dialog.setFixedSize(400, 300);  // 增加宽度和高度

    QVBoxLayout layout(&dialog);
    layout.setSpacing(15); // 增加控件之间的间距

    // 显示当前账号信息 - 增加高度并设置自动换行
    QLabel infoLabel(&dialog);
    infoLabel.setText(
        QString(tr("当前账号: %1\n账号ID: %2\n最高分: %3"))
            .arg(currentAccount.getUsername())
            .arg(currentAccount.getGameId())
            .arg(currentAccount.getHighScore())
        );
    infoLabel.setWordWrap(true); // 允许文本自动换行
    infoLabel.setMinimumHeight(120); // 设置最小高度
    infoLabel.setAlignment(Qt::AlignLeft | Qt::AlignTop); // 左上对齐
    infoLabel.setStyleSheet("font-size: 12px;"); // 设置字体大小
    layout.addWidget(&infoLabel);

    // 添加按钮 - 竖向排列
    QPushButton registerButton(tr("注册账号"), &dialog);
    QPushButton changeButton(tr("更换账号"), &dialog);
    QPushButton closeButton(tr("关闭"), &dialog);

    // 设置按钮固定大小和样式
    registerButton.setFixedSize(140, 40);
    changeButton.setFixedSize(140, 40);
    closeButton.setFixedSize(140, 40);

    // 设置按钮样式
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

    // 创建竖向按钮布局
    QVBoxLayout buttonLayout;
    buttonLayout.setSpacing(15); // 增加按钮之间的垂直间距
    buttonLayout.addStretch(); // 添加弹性空间
    buttonLayout.addWidget(&registerButton, 0, Qt::AlignCenter); // 居中
    buttonLayout.addWidget(&changeButton, 0, Qt::AlignCenter); // 居中
    buttonLayout.addWidget(&closeButton, 0, Qt::AlignCenter); // 居中
    buttonLayout.addStretch(); // 添加弹性空间

    layout.addLayout(&buttonLayout);

    // 连接按钮信号
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

    // 设置样式 - 黑色字体
    dialog.setStyleSheet("QLabel { color: black; font-size: 12px; } QPushButton { color: black; }");

    dialog.exec();
}
void MainWindow::registerAccount()
{
    // 创建注册对话框 - 增加高度
    QDialog dialog(this);
    dialog.setWindowTitle(tr("注册账号"));
    dialog.setFixedSize(400, 320);  // 增加宽度和高度

    QFormLayout form(&dialog);
    form.setSpacing(15); // 增加行间距
    form.setLabelAlignment(Qt::AlignRight); // 标签右对齐

    // 用户名输入
    QLineEdit usernameEdit;
    usernameEdit.setPlaceholderText(tr("请输入用户名"));
    usernameEdit.setMinimumHeight(30);
    form.addRow(tr("用户名:"), &usernameEdit);

    // 密码输入
    QLineEdit passwordEdit;
    passwordEdit.setPlaceholderText(tr("请输入密码"));
    passwordEdit.setEchoMode(QLineEdit::Password);
    passwordEdit.setMinimumHeight(30);
    form.addRow(tr("密码:"), &passwordEdit);

    // 确认密码输入
    QLineEdit confirmPasswordEdit;
    confirmPasswordEdit.setPlaceholderText(tr("请确认密码"));
    confirmPasswordEdit.setEchoMode(QLineEdit::Password);
    confirmPasswordEdit.setMinimumHeight(30);
    form.addRow(tr("确认密码:"), &confirmPasswordEdit);

    // 按钮 - 使用水平布局避免重叠
    QHBoxLayout buttonLayout;
    buttonLayout.setSpacing(20); // 增加按钮间距

    QPushButton okButton(tr("确定"), &dialog);
    QPushButton cancelButton(tr("取消"), &dialog);

    // 设置按钮固定大小和样式
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

    buttonLayout.addStretch(); // 添加弹性空间
    buttonLayout.addWidget(&okButton);
    buttonLayout.addWidget(&cancelButton);
    buttonLayout.addStretch(); // 添加弹性空间

    form.addRow("", new QWidget); // 空行
    form.addRow("", &buttonLayout); // 添加按钮布局

    // 连接按钮信号
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

        // 创建新账号
        Account newAccount;
        if (newAccount.createAccount(username, password)) {
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

    // 设置样式 - 黑色字体
    dialog.setStyleSheet("QLabel { color: black; font-size: 12px; } QLineEdit { color: black; font-size: 12px; } QPushButton { color: black; }");

    dialog.exec();
}

void MainWindow::changeAccount()
{
    // 创建更换账号对话框 - 增加高度
    QDialog dialog(this);
    dialog.setWindowTitle(tr("更换账号"));
    dialog.setFixedSize(400, 220);  // 增加宽度

    QFormLayout form(&dialog);
    form.setSpacing(15); // 增加行间距
    form.setLabelAlignment(Qt::AlignRight); // 标签右对齐

    // 账号ID输入
    QLineEdit accountIdEdit;
    accountIdEdit.setPlaceholderText(tr("请输入账号ID"));
    accountIdEdit.setMinimumHeight(30);
    form.addRow(tr("账号ID:"), &accountIdEdit);

    // 密码输入
    QLineEdit passwordEdit;
    passwordEdit.setPlaceholderText(tr("请输入密码"));
    passwordEdit.setEchoMode(QLineEdit::Password);
    passwordEdit.setMinimumHeight(30);
    form.addRow(tr("密码:"), &passwordEdit);

    // 按钮 - 使用水平布局避免重叠
    QHBoxLayout buttonLayout;
    buttonLayout.setSpacing(20); // 增加按钮间距

    QPushButton okButton(tr("确定"), &dialog);
    QPushButton cancelButton(tr("取消"), &dialog);

    // 设置按钮固定大小和样式
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

    buttonLayout.addStretch(); // 添加弹性空间
    buttonLayout.addWidget(&okButton);
    buttonLayout.addWidget(&cancelButton);
    buttonLayout.addStretch(); // 添加弹性空间

    form.addRow("", new QWidget); // 空行
    form.addRow("", &buttonLayout); // 添加按钮布局

    // 连接按钮信号
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

    // 设置样式 - 黑色字体
    dialog.setStyleSheet("QLabel { color: black; font-size: 12px; } QLineEdit { color: black; font-size: 12px; } QPushButton { color: black; }");

    dialog.exec();
}

void MainWindow::showMultiplayerDialog()
{
    // 显示多人游戏对话框
    QStringList items;
    items << tr("创建主机") << tr("连接主机");

    bool ok;
    QString choice = QInputDialog::getItem(
        this,
        tr("多人游戏"),
        tr("选择多人游戏模式:"),
        items,
        0,
        false,
        &ok
        );

    if (ok && !choice.isEmpty()) {
        if (choice == tr("创建主机")) {
            // 创建主机
            bool portOk;
            int port = QInputDialog::getInt(
                this,
                tr("创建主机"),
                tr("请输入端口号:"),
                12345,
                1024,
                65535,
                1,
                &portOk
                );

            if (portOk) {
                // 这里需要实现创建主机的逻辑
                QMessageBox::information(this, tr("创建主机"),
                                         tr("主机已创建在端口 %1").arg(port));
            }
        } else {
            // 连接主机
            bool ipOk;
            QString ip = QInputDialog::getText(
                this,
                tr("连接主机"),
                tr("请输入主机IP地址:"),
                QLineEdit::Normal,
                "127.0.0.1",
                &ipOk
                );

            if (ipOk && !ip.isEmpty()) {
                bool portOk;
                int port = QInputDialog::getInt(
                    this,
                    tr("连接主机"),
                    tr("请输入端口号:"),
                    12345,
                    1024,
                    65535,
                    1,
                    &portOk
                    );

                if (portOk) {
                    // 这里需要实现连接主机的逻辑
                    QMessageBox::information(this, tr("连接主机"),
                                             tr("正在连接 %1:%2").arg(ip).arg(port));
                }
            }
        }
    }
}


void MainWindow::showLeaderboard()
{
    Leaderboard *lb = Leaderboard::instance();
    QList<Account> topScores = lb->getTopScores(50); // 取前 50 名

    // 创建排行榜对话框
    QDialog dialog(this);
    dialog.setWindowTitle(tr("排行榜"));
    dialog.setFixedSize(500, 500);

    QVBoxLayout layout(&dialog);

    // 创建表格（4列：排名 / ID / 用户名 / 分数）
    QTableWidget table(topScores.size(), 4, &dialog);
    table.setHorizontalHeaderLabels(QStringList() << tr("排名") << tr("账号ID") << tr("用户名") << tr("分数"));
    table.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table.setEditTriggers(QAbstractItemView::NoEditTriggers);
    table.setSelectionBehavior(QAbstractItemView::SelectRows);

    // 填充表格数据
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

    // === 按钮区域 ===
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton clearButton(tr("清空排行榜"), &dialog);
    QPushButton closeButton(tr("关闭"), &dialog);

    buttonLayout->addWidget(&clearButton);
    buttonLayout->addWidget(&closeButton);

    layout.addLayout(buttonLayout);

    // 绑定按钮
    connect(&closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(&clearButton, &QPushButton::clicked, [&]() {
        lb->clear();
        QMessageBox::information(&dialog, tr("提示"), tr("排行榜已清空！"));
        dialog.accept(); // 关闭窗口
    });

    dialog.exec();
}

void MainWindow::on_singlePlayerButton_clicked()
{
    qDebug() << "Single-player button clicked";

    if (!gameWindow) {
        qDebug() << "Creating new GameWindow";
        gameWindow = new GameWindow(nullptr);
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
    // 显示设置对话框，包含账号管理和游戏设置
    QStringList items;
    items << tr("账号管理") << tr("游戏设置");

    bool ok;
    QString choice = QInputDialog::getItem(
        this,
        tr("设置"),
        tr("选择设置选项:"),
        items,
        0,
        false,
        &ok
        );

    if (ok && !choice.isEmpty()) {
        if (choice == tr("账号管理")) {
            showAccountDialog();
        } else {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle(tr("游戏设置"));
            msgBox.setText(tr("游戏设置将在后续版本中添加"));
            msgBox.setStyleSheet("QLabel { color: black; font-size: 12px; }");
            msgBox.exec();
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

    // 更新最高分
    if (gameWindow && gameWindow->getScore() > currentAccount.getHighScore()) {
        currentAccount.setHighScore(gameWindow->getScore());
        updateAccountDisplay();

        // 更新排行榜
        Leaderboard *lb = Leaderboard::instance();
        lb->updateAccount(currentAccount);
        lb->save();
    }

    // 显示排行榜
    showLeaderboard();
}
