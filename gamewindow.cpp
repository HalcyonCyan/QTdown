#include "gamewindow.h"
#include <QKeyEvent>
#include <QPainter>
#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>

// 修改后的构造函数：支持多人模式和网络
GameWindow::GameWindow(bool isMultiplayer, Network* network, QWidget *parent)
    : QWidget(parent), showDebugInfo(false), gamePaused(false),
    network(network), isMultiplayerMode(isMultiplayer)
{
    // 创建游戏引擎，传递网络参数
    gameEngine = new GameEngine(width(), height(), isMultiplayer, network, this);

    // 设置定时器
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameWindow::updateGame);

    // 连接网络错误信号（如果是多人游戏）
    if (isMultiplayerMode && network) {
        connect(network, &Network::errorOccurred, this, &GameWindow::onNetworkError);
    }

    // 设置窗口属性
    setFocusPolicy(Qt::StrongFocus);
    setFixedSize(640, 500);
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

int GameWindow::getScore() const
{
    return gameEngine->getScore();
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
void GameWindow::setNetwork(Network* network, bool isHost)
{
    // 清理旧的网络连接
    if (this->network) {
        disconnect(this->network, &Network::playerPositionReceived,
                   gameEngine, &GameEngine::onPlayerPositionReceived);
        disconnect(this->network, &Network::gameStateReceived,
                   gameEngine, &GameEngine::onGameStateReceived);
        disconnect(this->network, &Network::scoreUpdateReceived,
                   gameEngine, &GameEngine::onScoreUpdateReceived);
        disconnect(this->network, &Network::errorOccurred,
                   this, &GameWindow::onNetworkError);
    }

    // 设置新的网络对象
    this->network = network;
    this->isMultiplayerMode = isHost; // 或者 isMultiplayerMode = true;

    // 重新连接信号和槽
    if (network) {
        connect(network, &Network::playerPositionReceived,
                gameEngine, &GameEngine::onPlayerPositionReceived);
        connect(network, &Network::gameStateReceived,
                gameEngine, &GameEngine::onGameStateReceived);
        connect(network, &Network::scoreUpdateReceived,
                gameEngine, &GameEngine::onScoreUpdateReceived);
        connect(network, &Network::errorOccurred,
                this, &GameWindow::onNetworkError);
    }

    // 更新游戏引擎的网络状态
    if (gameEngine) {
        gameEngine->setNetwork(network, isHost);
    }
}

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
    update();
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

// 新增：网络错误处理槽函数
void GameWindow::onNetworkError(const QString &error)
{
    gamePaused = true;
    timer->stop();

    QMessageBox::warning(this, tr("网络错误"),
                         tr("网络连接出现问题: %1").arg(error));
    update();
}
