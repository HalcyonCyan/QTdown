#include "gamewindow.h"
#include <QKeyEvent>
#include <QPainter>
#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>


GameWindow::GameWindow(bool isMultiplayer, Network* network, QWidget *parent)
    : QWidget(parent), showDebugInfo(false), gamePaused(false),
    network(network), isMultiplayerMode(isMultiplayer), isHost(false)
{
    
    gameEngine = new GameEngine(width(), height(), isMultiplayer, network, this);

    
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameWindow::updateGame);

    
    if (isMultiplayerMode && network) {
        connect(network, &Network::errorOccurred, this, &GameWindow::onNetworkError);
        connect(network, &Network::disconnected, this, &GameWindow::onNetworkDisconnected);
    }

    
    setFocusPolicy(Qt::StrongFocus);
    setFixedSize(640, 500);
}

GameWindow::~GameWindow()
{
    delete gameEngine;
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

    
    QPixmap background(":/ima/background.jpg");
    if (!background.isNull()) {
        
        painter.drawPixmap(rect(), background.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
        
        painter.fillRect(rect(), QColor(250, 200, 255));
    }

    if (isMultiplayerMode && gameEngine->isMultiplayer() &&
        gameEngine->isWaitingForInitialData()) {  
        painter.setBrush(QColor(0, 0, 0, 150));
        painter.setPen(Qt::NoPen);
        painter.drawRect(rect());

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 16));
        painter.drawText(rect(), Qt::AlignCenter, tr("Waiting for host data..."));
        return;
    }

    
    gameEngine->draw(painter, showDebugInfo);

    
    drawScore(painter);

    
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
    
    if (this->network) {
        disconnect(this->network, &Network::playerPositionReceived,
                   gameEngine, &GameEngine::onPlayerPositionReceived);
        disconnect(this->network, &Network::gameStateReceived,
                   gameEngine, &GameEngine::onGameStateReceived);
        disconnect(this->network, &Network::scoreUpdateReceived,
                   gameEngine, &GameEngine::onScoreUpdateReceived);
        disconnect(this->network, &Network::platformDataReceived,
                   gameEngine, &GameEngine::onPlatformDataReceived);
        disconnect(this->network, &Network::gameInitDataReceived,
                   gameEngine, &GameEngine::onGameInitDataReceived);
        disconnect(this->network, &Network::errorOccurred,
                   this, &GameWindow::onNetworkError);
    }

    
    this->network = network;
    this->isMultiplayerMode = true;
    this->isHost = isHost;

    
    if (network) {
        connect(network, &Network::playerPositionReceived,
                gameEngine, &GameEngine::onPlayerPositionReceived);
        connect(network, &Network::gameStateReceived,
                gameEngine, &GameEngine::onGameStateReceived);
        connect(network, &Network::scoreUpdateReceived,
                gameEngine, &GameEngine::onScoreUpdateReceived);
        connect(network, &Network::platformDataReceived,
                gameEngine, &GameEngine::onPlatformDataReceived);
        connect(network, &Network::gameInitDataReceived,
                gameEngine, &GameEngine::onGameInitDataReceived);
        connect(network, &Network::errorOccurred,
                this, &GameWindow::onNetworkError);
        connect(network, &Network::disconnected,
                this, &GameWindow::onNetworkDisconnected);
    }

    
    if (gameEngine) {
        gameEngine->setNetwork(network, isHost);
    }
}


void GameWindow::onNetworkError(const QString &error)
{
    gamePaused = true;
    timer->stop();

    
    QTimer::singleShot(0, this, [this, error]() {
        QMessageBox::warning(this, tr("网络错误"),
                             tr("网络连接出现问题: %1").arg(error));
        update();
    });
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

void GameWindow::onNetworkDisconnected()
{
    gamePaused = true;
    timer->stop();

    QMessageBox::warning(this, tr("连接断开"),
                         tr("与对方的连接已断开，游戏将暂停"));
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



