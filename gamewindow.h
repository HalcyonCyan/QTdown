#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include "gameengine.h"
#include "network.h"

class GameWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GameWindow(bool isMultiplayer = false, Network* network = nullptr,
                        QWidget *parent = nullptr);
    ~GameWindow();
    GameEngine* getGameEngine() const { return gameEngine; }
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
    void onNetworkError(const QString &error);
    void onNetworkDisconnected();

private:
    void drawScore(QPainter &painter);
    void toggleDebugInfo();
    void drawPauseMenu(QPainter &painter);
    void returnToMainMenu();

    QTimer *timer;
    GameEngine *gameEngine;
    bool showDebugInfo;
    bool gamePaused;

    
    Network* network;
    bool isMultiplayerMode;
    bool isHost;
};

#endif 
