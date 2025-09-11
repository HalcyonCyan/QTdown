#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include "gameengine.h"
#include "network.h"  // 添加网络头文件

class GameWindow : public QWidget
{
    Q_OBJECT

public:
    // 修改构造函数，添加网络参数
    explicit GameWindow(bool isMultiplayer = false, Network* network = nullptr,
                        QWidget *parent = nullptr);
    ~GameWindow();

    void startGame();
    void restartGame();
    int getScore() const;
    void setNetwork(Network* network, bool isHost);
signals:
    void gameFinished();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void updateGame();
    void onNetworkError(const QString &error);  // 添加网络错误处理

private:
    void drawScore(QPainter &painter);
    void toggleDebugInfo();
    void drawPauseMenu(QPainter &painter);
    void returnToMainMenu();
    bool isHost;
    Network* network;
    int score;

    // 其他成员变量和函数
    void initialize(); // 示例初始化函数
\

    // 连接信号和槽
    void connectNetworkSignals();
    QTimer *timer;
    GameEngine *gameEngine;
    bool showDebugInfo;
    bool gamePaused;

    // 添加网络相关成员

    bool isMultiplayerMode;
};

#endif // GAMEWINDOW_H
