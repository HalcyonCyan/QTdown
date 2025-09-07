#include "gamewindow.h"
#include <QKeyEvent>
#include <QPainter>
#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent), showDebugInfo(false), gamePaused(false)
{
    // 创建游戏引擎
    gameEngine = new GameEngine(width(), height(), this);

    // 设置定时器
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameWindow::updateGame);

    // 设置窗口属性
    setFocusPolicy(Qt::StrongFocus);
    setFixedSize(400, 500);
}

GameWindow::~GameWindow()
{
    delete gameEngine;
}

void GameWindow::startGame()
{
    gameEngine->initGame();
    timer->start(8);
    gamePaused = false;
}

void GameWindow::restartGame()
{
    gameEngine->initGame();
    timer->start(8);
    gamePaused = false;
}

void GameWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    painter.fillRect(rect(), QColor(250, 200, 255));

    // 绘制游戏内容
    gameEngine->draw(painter, showDebugInfo);

    // 绘制分数
    drawScore(painter);

    // 如果游戏暂停或结束，绘制菜单
    if (gamePaused || gameEngine->isGameOver()) {
        drawPauseMenu(painter);
    }
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
    if (gamePaused || gameEngine->isGameOver()) {
        switch (event->key()) {
        case Qt::Key_R:
            restartGame();
            break;
        case Qt::Key_M:
            returnToMainMenu();
            break;
        case Qt::Key_E:
            if (gamePaused) {
                gamePaused = false;
                timer->start(8);
            } else {
                gamePaused = true;
                timer->stop();
            }
            update();
            break;
        }
        return;
    }

    switch (event->key()) {
    case Qt::Key_Left:
        gameEngine->movePlayerLeft();
        break;
    case Qt::Key_Right:
        gameEngine->movePlayerRight();
        break;
    case Qt::Key_Space:
        gameEngine->jumpPlayer();
        break;
    case Qt::Key_Up:
        gameEngine->jumpPlayer();
        break;
    case Qt::Key_D:
        toggleDebugInfo();
        break;
    case Qt::Key_Escape:
        gamePaused = true;
        timer->stop();
        update();
        break;
    case Qt::Key_R:
        restartGame();
        break;
    }
}

// 添加按键释放事件处理
void GameWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (gamePaused || gameEngine->isGameOver()) {
        return;
    }

    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right:
        gameEngine->stopPlayerMovement();
        break;
    }
}

void GameWindow::updateGame()
{
    if (!gameEngine->isGameOver()) {
        gameEngine->update();
    } else {
        timer->stop();
    }
    update(); // 触发重绘
}


void GameWindow::drawScore(QPainter &painter)
{
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 16));
    painter.drawText(10, 30, QString(tr("Score: %1")).arg(gameEngine->getScore()));

    int yOffset = 60;
    if (gameEngine->isBoostActive()) {
        painter.setPen(Qt::red);
        painter.drawText(10, yOffset, tr("Boost Active!"));
        yOffset += 30;
    }

    if (gameEngine->isSlowActive()) {
        painter.setPen(Qt::blue);
        painter.drawText(10, yOffset, tr("Slow Active!"));
        yOffset += 30;
    }

    if (gameEngine->isBounceActive()) {
        painter.setPen(Qt::green);
        painter.drawText(10, yOffset, tr("Bounce Active!"));
    }
}

void GameWindow::drawPauseMenu(QPainter &painter)
{
    // 绘制半透明背景
    painter.setBrush(QColor(0, 0, 0, 150));
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 24, QFont::Bold));

    if (gameEngine->isGameOver()) {
        painter.drawText(rect().adjusted(0, 0, 0, -100), Qt::AlignCenter, tr("Game Over"));
        painter.setFont(QFont("Arial", 16));
        painter.drawText(rect().adjusted(0, 50, 0, 0), Qt::AlignCenter,
                         QString(tr("Final Score: %1")).arg(gameEngine->getScore()));
    } else {
        painter.drawText(rect().adjusted(0, 0, 0, -100), Qt::AlignCenter, tr("Game Paused"));
    }

    painter.setFont(QFont("Arial", 14));
    QStringList options;
    options << tr("Press R to Restart") << tr("Press M to Return to Menu") << tr("Press E to Continue");

    for (int i = 0; i < options.size(); i++) {
        painter.drawText(rect().adjusted(0, 150 + i * 30, 0, 0), Qt::AlignCenter, options[i]);
    }
}

void GameWindow::returnToMainMenu()
{
    timer->stop();
    emit gameFinished();
}

void GameWindow::toggleDebugInfo()
{
    showDebugInfo = !showDebugInfo;
}
