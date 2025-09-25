#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gamewindow.h"
#include "account.h"
#include "network.h" 
#include "waitingdialog.h"
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
    void onHostCreated(const QString &ip, quint16 port);

private:
    Ui::MainWindow *ui;
    GameWindow *gameWindow;
    bool gameRunning;
    Account currentAccount;    WaitingDialog *waitingDialog;


    void updateAccountDisplay();
    void showAccountDialog();
    void registerAccount();    
    void changeAccount();      
    void showMultiplayerDialog();
    void startMultiplayerGame(Network* network, bool isHost); 
    void showLeaderboard();

};

#endif 
