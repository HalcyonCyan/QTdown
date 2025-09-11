#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gamewindow.h"
#include "account.h"
#include "network.h" // 添加Network头文件

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_singlePlayerButton_clicked();
    void on_multiplayerButton_clicked();
    void on_settingsButton_clicked();
    void on_leaderboardButton_clicked();
    void on_exitButton_clicked();
    void gameFinished();

private:
    Ui::MainWindow *ui;
    GameWindow *gameWindow;
    bool gameRunning;
    Account currentAccount;

    void updateAccountDisplay();
    void showAccountDialog();
    void registerAccount();    // 添加声明
    void changeAccount();      // 添加声明
    void showMultiplayerDialog();
    void startMultiplayerGame(Network* network, bool isHost); // 添加声明
    void showLeaderboard();
};

#endif // MAINWINDOW_H
