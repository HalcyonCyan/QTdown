#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include "gameengine.h"

class GameWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

    void startGame();
    void restartGame();
    int getScore() const;

signals:
    void gameFinished();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void updateGame();

private:
    void drawScore(QPainter &painter);
    void toggleDebugInfo();
    void drawPauseMenu(QPainter &painter);
    void returnToMainMenu();

    QTimer *timer;
    GameEngine *gameEngine;
    bool showDebugInfo;
    bool gamePaused;
};

#endif // GAMEWINDOW_H
