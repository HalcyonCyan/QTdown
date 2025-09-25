#include "gameengine.h"
#include <QDebug>
#include <QRandomGenerator>
#include <algorithm>
#include <climits>
#include <QPainter>
#include <QDateTime>
#include <cmath>

GameEngine::GameEngine(int width, int height, bool isMultiplayer, Network* network, QObject *parent)
    : QObject(parent), width(width), height(height), score(0), scrollOffset(0), gameOver(false),
    boostActive(false), boostTimer(0), slowActive(false), slowTimer(0),
    bounceActive(false), bounceTimer(0), initTime(0), lastUpdateTime(0), deltaTime(0),
    targetVelocityX(0), currentVelocityX(0), gameTime(0),
    network(network), isMultiplayerMode(isMultiplayer), isHost(false), remotePlayer(nullptr),
    lastPositionUpdateTime(0), waitingForInitialData(false) 
{
    player = new Player(width / 2, height / 2, 24, 30);

    
    if (isMultiplayerMode) {
        remotePlayer = new Player(width / 2 - 50, height / 2, 24, 30);
    }else {
        remotePlayer = nullptr; 
    }

    platformWidth = 90;
    platformHeight = 14;
    platformMinGap = 30;
    platformMaxGap = 80;
    platformScrollSpeed = 1;

    batchSize = 10;
    batchThreshold = 7;

    initGame();

    if (network) {
        connect(network, &Network::playerPositionReceived, this, &GameEngine::onPlayerPositionReceived);
        connect(network, &Network::gameStateReceived, this, &GameEngine::onGameStateReceived);
        connect(network, &Network::scoreUpdateReceived, this, &GameEngine::onScoreUpdateReceived);
        connect(network, &Network::platformDataReceived, this, &GameEngine::onPlatformDataReceived);
        connect(network, &Network::gameInitDataReceived, this, &GameEngine::onGameInitDataReceived);
    }
}
bool GameEngine::checkPlayerCollision() const {
    if (!player || !remotePlayer) return false;

    
    QRectF playerRect(player->getX() - player->getWidth() / 2,
                      player->getY() - player->getHeight() / 2,
                      player->getWidth(),
                      player->getHeight());

    QRectF remotePlayerRect(remotePlayer->getX() - remotePlayer->getWidth() / 2,
                            remotePlayer->getY() - remotePlayer->getHeight() / 2,
                            remotePlayer->getWidth(),
                            remotePlayer->getHeight());

    
    return playerRect.intersects(remotePlayerRect);
}
void GameEngine::initGame()
{
    
    player->reset(width / 2, height / 2);

    qDeleteAll(platforms);
    platforms.clear();
    qDeleteAll(platformBatch);
    platformBatch.clear();
    recentPlatformXs.clear();

    
    if (isMultiplayerMode && !isHost) {
        
        return;
    }

    generateInitialPlatforms();

    
    generatePlatformBatch();

    score = 0;
    scrollOffset = 0;
    gameOver = false;

    
    boostActive = false;
    boostTimer = 0;
    slowActive = false;
    slowTimer = 0;
    bounceActive = false;
    bounceTimer = 0;

    
    targetVelocityX = 0;
    currentVelocityX = 0;

    
    initTime = QDateTime::currentMSecsSinceEpoch();
    lastUpdateTime = initTime;
    gameTime = 0;

    
    if (isMultiplayerMode && isHost && network) {
        QByteArray gameState;
        serializeGameState(gameState);
        network->sendGameState(gameState);
    }
    if (isMultiplayerMode && network && !isHost) {
        waitingForInitialData = true;
    }
}
GameEngine::~GameEngine()
{
    delete player;
    qDeleteAll(platforms);
    platforms.clear();
    qDeleteAll(platformBatch);
    platformBatch.clear();
}


void GameEngine::onPlatformDataReceived(const QByteArray& data)
{
    if (isMultiplayerMode && !isHost) {
        deserializePlatforms(data);
        waitingForInitialData = false; 
    }
}

void GameEngine::onGameInitDataReceived(const QByteArray& data)
{
    if (isMultiplayerMode && !isHost) {
        deserializeGameState(data);
        
        initTime = QDateTime::currentMSecsSinceEpoch();
        lastUpdateTime = initTime;
        waitingForInitialData = false; 
    }
}

void GameEngine::update()
{
    if (isMultiplayerMode && !isHost && waitingForInitialData) {
        return;
    }
    
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    if (lastUpdateTime == 0) {
        lastUpdateTime = currentTime;
    }
    deltaTime = (currentTime - lastUpdateTime) / 1000.0; 
    lastUpdateTime = currentTime;

    
    gameTime = static_cast<int>((currentTime - initTime) );

    
    if (deltaTime > 0.1) {
        deltaTime = 0.1;
    }

    
    updateHorizontalMovement();

    
    if (boostActive) {
        boostTimer -= deltaTime * 60;
        if (boostTimer <= 0) {
            boostActive = false;
        }
    }

    if (slowActive) {
        slowTimer -= deltaTime * 60;
        if (slowTimer <= 0) {
            slowActive = false;
        }
    }

    if (bounceActive) {
        bounceTimer -= deltaTime * 60;
        if (bounceTimer <= 0) {
            bounceActive = false;
        }
    }

    float bei = std::min(1.0 + gameTime * 1.0 / 200000 + score*1.0 / 3000, 1.5);

    
    bool wasOnPlatform = player->isOnPlatform();
    player->setOnPlatform(false);

    
    player->applyGravity(0.33 * deltaTime * 60 * bei);

    
    scrollOffset += platformScrollSpeed * deltaTime * 220 * bei;

    
    checkCollisions();

    
    if (wasOnPlatform && !player->isOnPlatform()) {
        player->setJumping(true);
    }

    
    int highestPlatformY = 0;
    for (Platform *platform : platformBatch) {
        if (platform->getY() > highestPlatformY) {
            highestPlatformY = platform->getY();
        }
    }

    if (isMultiplayerMode && network && network->isConnected()) {
        
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        if (currentTime - lastPositionUpdateTime > positionUpdateInterval) {
            network->sendPlayerPosition(player->getX(), player->getY(), player->getVelocity());
            lastPositionUpdateTime = currentTime;
        }

        
        if (!remotePlayer) {
            remotePlayer = new Player(width / 2 - 50, height / 2, 24, 30);
        }

        
        if (checkPlayerCollision()) {
            
        }
    }

    
    int highestScreenY = highestPlatformY - scrollOffset;
    if (highestScreenY < height + 200) { 
        generatePlatformBatch();
    }

    
    for (int i = platformBatch.size() - 1; i >= 0; i--) {
        Platform *platform = platformBatch[i];
        int platformScreenY = platform->getY() - scrollOffset;
        if (platformScreenY < height+100) {
            platforms.append(platform);
            platformBatch.removeAt(i);
        }
    }

    
    removeOffscreenPlatforms();

    
    checkGameOver();
}


void GameEngine::serializePlatforms(QByteArray& data) const
{
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << platforms.size();
    for (Platform* platform : platforms) {
        stream << platform->getX() << platform->getY() << platform->getWidth()
        << platform->getHeight() << static_cast<int>(platform->getType())
        << platform->isPassed();
    }

    stream << platformBatch.size();
    for (Platform* platform : platformBatch) {
        stream << platform->getX() << platform->getY() << platform->getWidth()
        << platform->getHeight() << static_cast<int>(platform->getType())
        << platform->isPassed();
    }

    stream << recentPlatformXs;
    stream << scrollOffset;
}

void GameEngine::deserializePlatforms(const QByteArray& data)
{
    QDataStream stream(data);

    
    qDeleteAll(platforms);
    platforms.clear();
    qDeleteAll(platformBatch);
    platformBatch.clear();
    recentPlatformXs.clear();

    int platformCount, batchCount;
    stream >> platformCount;

    for (int i = 0; i < platformCount; i++) {
        int x, y, width, height, typeInt;
        bool passed;
        stream >> x >> y >> width >> height >> typeInt >> passed;

        Platform::PlatformType type = static_cast<Platform::PlatformType>(typeInt);
        Platform* platform = new Platform(x, y, width, height, passed, type);
        platforms.append(platform);
    }

    stream >> batchCount;
    for (int i = 0; i < batchCount; i++) {
        int x, y, width, height, typeInt;
        bool passed;
        stream >> x >> y >> width >> height >> typeInt >> passed;

        Platform::PlatformType type = static_cast<Platform::PlatformType>(typeInt);
        Platform* platform = new Platform(x, y, width, height, passed, type);
        platformBatch.append(platform);
    }

    stream >> recentPlatformXs;
    stream >> scrollOffset;
}

void GameEngine::serializeGameState(QByteArray& data) const
{
    QDataStream stream(&data, QIODevice::WriteOnly);
    serializePlatforms(data);
    stream << score << gameOver << boostActive << boostTimer
           << slowActive << slowTimer << bounceActive << bounceTimer
           << gameTime;
}

void GameEngine::deserializeGameState(const QByteArray& data)
{
    QDataStream stream(data);

    
    deserializePlatforms(data);

    
    stream >> score >> gameOver >> boostActive >> boostTimer
        >> slowActive >> slowTimer >> bounceActive >> bounceTimer
        >> gameTime;
}


void GameEngine::updateHorizontalMovement()
{
    
    if (targetVelocityX != 0) {
        
        if (currentVelocityX * targetVelocityX >= 0) {
            
            float effectiveAcceleration = acceleration;

            
            if (slowActive) {
                effectiveAcceleration *= 0.5f;
            }

            
            if (boostActive) {
                effectiveAcceleration *= 1.5f;
            }

            currentVelocityX += targetVelocityX * effectiveAcceleration * deltaTime * 60;

            
            float maxSpeed = maxNormalSpeed;
            if (boostActive) maxSpeed = maxBoostSpeed;
            if (slowActive) maxSpeed *= 0.6f; 

            if (fabs(currentVelocityX) > maxSpeed) {
                currentVelocityX = (currentVelocityX > 0) ? maxSpeed : -maxSpeed;
            }
        } else {
            
            float effectiveAcceleration = acceleration * 2;

            
            if (slowActive) {
                effectiveAcceleration *= 0.5f;
            }

            
            if (boostActive) {
                effectiveAcceleration *= 1.5f;
            }

            currentVelocityX += targetVelocityX * effectiveAcceleration * deltaTime * 60;
        }
    } else {
        
        float effectiveDeceleration = deceleration;

        
        if (slowActive) {
            effectiveDeceleration *= 1.5f;
        }

        
        if (boostActive) {
            effectiveDeceleration *= 0.5f;
        }

        if (currentVelocityX > 0) {
            currentVelocityX -= effectiveDeceleration * deltaTime * 60;
            if (currentVelocityX < 0) currentVelocityX = 0;
        } else if (currentVelocityX < 0) {
            currentVelocityX += effectiveDeceleration * deltaTime * 60;
            if (currentVelocityX > 0) currentVelocityX = 0;
        }
    }

    
    if (fabs(currentVelocityX) > 0.1f) {
        player->move(currentVelocityX * deltaTime * 60, 0, width);
    }
}

void GameEngine::generateInitialPlatforms()
{
    
    Platform *startingPlatform = new Platform(
        player->getX() - platformWidth / 2,
        player->getY() + player->getHeight() / 2,
        platformWidth,
        platformHeight,
        true
        );
    platforms.append(startingPlatform);
    recentPlatformXs.append(startingPlatform->getX());

    
    int currentY = player->getY() + player->getHeight() / 2 + platformMinGap;
    for (int i = 0; i < 12; i++) { 
        addPlatformToBatch(currentY);
        currentY += QRandomGenerator::global()->bounded(platformMinGap, platformMaxGap);
    }
}





void GameEngine::generatePlatformBatch()
{
    
    if (isMultiplayerMode && !isHost) {
        return;
    }

    
    int highestY = 0;
    for (Platform *platform : platforms) {
        if (platform->getY() > highestY) {
            highestY = platform->getY();
        }
    }
    for (Platform *platform : platformBatch) {
        if (platform->getY() > highestY) {
            highestY = platform->getY();
        }
    }

    
    int currentY = highestY + QRandomGenerator::global()->bounded(platformMinGap, platformMaxGap);
    for (int i = 0; i < batchSize; i++) {
        addPlatformToBatch(currentY);
        currentY += QRandomGenerator::global()->bounded(platformMinGap, platformMaxGap);
    }

    
    if (isMultiplayerMode && isHost && network) {
        QByteArray platformData;
        serializePlatforms(platformData);
        network->sendPlatformData(platformData);
    }
}

void GameEngine::addPlatformToBatch(int y)
{
    int newX;
    int platformWidthToUse = platformWidth;

    
    int averageX = 0;
    if (!recentPlatformXs.isEmpty()) {
        for (int x : recentPlatformXs) {
            averageX += x;
        }
        averageX /= recentPlatformXs.size();
    } else {
        averageX = width / 2;
    }

    
    int baseProbability = 2;
    int redProbability = baseProbability + std::min(score / 80, 7);
    int blueProbability = baseProbability + std::min(score / 80, 12);
    int greenProbability = std::min(score / 110, 7);

    int randValue = QRandomGenerator::global()->bounded(100);
    Platform::PlatformType type = Platform::NORMAL;

    qDebug() << "Platform generation - randValue:" << randValue
             << "redProb:" << redProbability
             << "blueProb:" << blueProbability
             << "greenProb:" << greenProbability;

    if (randValue < redProbability) {
        type = Platform::BOOST;
        qDebug() << "生成红色平台";
    } else if (randValue < redProbability + blueProbability) {
        type = Platform::SLOW;
        qDebug() << "生成蓝色平台";
    } else if (randValue < redProbability + blueProbability + greenProbability) {
        type = Platform::BOUNCE;
        qDebug() << "生成青色平台";
    } else {
        qDebug() << "生成普通平台";
    }

    
    if (type == Platform::BOOST) {
        platformWidthToUse = 60 + QRandomGenerator::global()->bounded(100) / 10;
    } else if (type == Platform::SLOW || type == Platform::BOUNCE) {
        platformWidthToUse = 70 + QRandomGenerator::global()->bounded(60) / 10;
    }

    
    if (QRandomGenerator::global()->bounded(100) < 20) {
        
        int gap = std::abs(averageX - 200) / 2 + 3;

        
        bool generateOnRight = averageX < 200;

        if (generateOnRight) {
            
            if (QRandomGenerator::global()->bounded(100) < 50)
                gap = 0;
            int x1 = QRandomGenerator::global()->bounded(width / 2 - 50 - gap);
            int x2 = width / 2 + gap + QRandomGenerator::global()->bounded(width / 2 - 50 - gap);

            
            x1 = std::max(0, std::min(x1, width - 50));
            x2 = std::max(0, std::min(x2, width - 50));

            Platform *platform1 = new Platform(x1, y, 50, platformHeight, false, type);
            Platform *platform2 = new Platform(x2, y, 50, platformHeight, false, type);

            platformBatch.append(platform1);
            platformBatch.append(platform2);

            
            recentPlatformXs.append((x1 + x2) / 2);
            if (QRandomGenerator::global()->bounded(100) < 80) {
                qDebug()<<"1";
                Platform* targetPlatform = (QRandomGenerator::global()->bounded(100) < 50) ? platform1 : platform2;
                FloatingObject obj;
                obj.parentPlatform = targetPlatform;
                obj.radius = 8.0f;
                qDebug()<<"2";
                float randomX = targetPlatform->getX() + QRandomGenerator::global()->bounded(targetPlatform->getWidth() - obj.radius * 2);
                obj.position = QPointF(randomX + obj.radius, y - 20);
                obj.active = true;
                floatingObjects.append(obj);
            }
        } else {
            
            if (QRandomGenerator::global()->bounded(100) < 40)
                gap = 0;
            int x1 = QRandomGenerator::global()->bounded(width / 2 - 50 - gap);
            int x2 = width / 2 + gap + QRandomGenerator::global()->bounded(width / 2 - 50 - gap);

            
            x1 = std::max(0, std::min(x1, width - 50));
            x2 = std::max(0, std::min(x2, width - 50));

            Platform *platform1 = new Platform(x1, y, 50, platformHeight, false, type);
            Platform *platform2 = new Platform(x2, y, 50, platformHeight, false, type);

            platformBatch.append(platform1);
            platformBatch.append(platform2);

            
            recentPlatformXs.append((x1 + x2) / 2);
        }
    } else {
        
        bool generateOnRight = averageX < width / 2;
        if( QRandomGenerator::global()->bounded(60)<20)
        {
            int x=width / 2 - platformWidthToUse - 40;
            newX=width / 2+ QRandomGenerator::global()->bounded(2 * x + 1) - x;
        } else if (generateOnRight) {
          
          int rightRange = width / 2 - platformWidthToUse - 20;
          if (rightRange > 0) {
            newX = width / 2 + QRandomGenerator::global()->bounded(rightRange);
          } else {
            newX = width / 2;
          }
        } else {
          
          int leftRange = width / 2 - platformWidthToUse - 20;
          if (leftRange > 0) {
            newX = QRandomGenerator::global()->bounded(leftRange);
          } else {
            newX = 0;
          }
        }

        
        newX = std::max(0, std::min(newX, width - platformWidthToUse));
        Platform *singlePlatform = new Platform(newX, y, platformWidthToUse, platformHeight, false, type);
        Platform *platform = new Platform(newX, y, platformWidthToUse, platformHeight, false, type);
        platformBatch.append(platform);

        
        recentPlatformXs.append(newX);
        if (QRandomGenerator::global()->bounded(100) < 10) {
            FloatingObject obj;
            obj.parentPlatform = singlePlatform;
            obj.radius = 8.0f;
            float randomX = newX + QRandomGenerator::global()->bounded(platformWidthToUse - obj.radius * 2);
            obj.position = QPointF(randomX + obj.radius, y - 20);
            obj.active = true;
            floatingObjects.append(obj);
        }
    }

    
    if (recentPlatformXs.size() > recentPlatformCount) {
        recentPlatformXs.removeFirst();
    }
}

void GameEngine::draw(QPainter &painter, bool showDebugInfo)
{
    
    painter.setPen(Qt::black);
    for (Platform *platform : platforms) {
        int platformScreenY = platform->getY() - scrollOffset;
        if (platformScreenY > -platformHeight && platformScreenY < height+100) {
            
            switch(platform->getType()) {
            case Platform::BOUNCE:
                painter.setBrush(QColor(20, 250, 250)); 
                qDebug() << "Drawing BOUNCE platform at y:" << platformScreenY;
                break;
            case Platform::BOOST:
                painter.setBrush(Qt::red);
                qDebug() << "Drawing BOOST platform at y:" << platformScreenY;
                break;
            case Platform::SLOW:
                painter.setBrush(Qt::blue);
                qDebug() << "Drawing SLOW platform at y:" << platformScreenY;
                break;
            default:
                painter.setBrush(QColor(100, 200, 100));
                break;
            }

            painter.drawRect(platform->getX(), platformScreenY, platform->getWidth(), platform->getHeight());

            
            if (showDebugInfo) {
                painter.setPen(Qt::red);
                painter.drawLine(platform->getX(), platformScreenY,
                                 platform->getX() + platform->getWidth(), platformScreenY);
                painter.setPen(Qt::black);
            }
        }
    }

    
    for (Platform *platform : platformBatch) {
        int platformScreenY = platform->getY() - scrollOffset;
        if (platformScreenY > -platformHeight && platformScreenY < height+100) {
            
            QColor platformColor;

            switch(platform->getType()) {
            case Platform::BOOST:
                platformColor = Qt::red;
                qDebug() << "Drawing batch BOOST platform at y:" << platformScreenY;
                break;
            case Platform::SLOW:
                platformColor = Qt::blue;
                qDebug() << "Drawing batch SLOW platform at y:" << platformScreenY;
                break;
            case Platform::BOUNCE:
                platformColor = QColor(20, 250, 250); 
                qDebug() << "Drawing batch BOUNCE platform at y:" << platformScreenY;
                break;
            default:
                platformColor = QColor(100, 200, 100);
                break;
            }

            platformColor.setAlpha(128); 
            painter.setBrush(platformColor);
            painter.setPen(Qt::black);
            painter.drawRect(platform->getX(), platformScreenY, platform->getWidth(), platform->getHeight());

            
            if (showDebugInfo) {
                painter.setPen(Qt::red);
                painter.drawLine(platform->getX(), platformScreenY,
                                 platform->getX() + platform->getWidth(), platformScreenY);
                painter.setPen(Qt::black);
            }
        }
    }

    
    player->draw(painter, scrollOffset);

    
    if (boostActive) {
        painter.setBrush(QColor(255, 100, 100, 100));
        painter.setPen(Qt::NoPen);
        int playerScreenY = player->getY() - scrollOffset;
        painter.drawEllipse(player->getX() - player->getWidth() / 2 - 5,
                            playerScreenY - player->getHeight() / 2 - 5,
                            player->getWidth() + 10,
                            player->getHeight() + 10);
    }

    if (slowActive) {
        painter.setBrush(QColor(100, 100, 255, 100));
        painter.setPen(Qt::NoPen);
        int playerScreenY = player->getY() - scrollOffset;
        painter.drawRect(player->getX() - player->getWidth() / 2 - 5,
                         playerScreenY - player->getHeight() / 2 - 5,
                         player->getWidth() + 10,
                         player->getHeight() + 10);
    }

    if (bounceActive) {
        painter.setBrush(QColor(100, 255, 100, 100));
        painter.setPen(Qt::NoPen);
        int playerScreenY = player->getY() - scrollOffset;
        QPointF points[4] = {
            QPointF(player->getX(), playerScreenY - player->getHeight() / 2 - 8),
            QPointF(player->getX() - player->getWidth() / 2 - 5, playerScreenY + player->getHeight() / 2 + 5),
            QPointF(player->getX() + player->getWidth() / 2 + 5, playerScreenY + player->getHeight() / 2 + 5),
            QPointF(player->getX(), playerScreenY - player->getHeight() / 2 - 8)
        };
        painter.drawPolygon(points, 4);
    }
    if (isMultiplayerMode && remotePlayer) {
        
        painter.setBrush(QColor(255, 100, 100)); 
        painter.setPen(Qt::black);

        int playerScreenY = remotePlayer->getY() - scrollOffset;
        painter.drawRect(remotePlayer->getX() - remotePlayer->getWidth() / 2,
                         playerScreenY - remotePlayer->getHeight() / 2,
                         remotePlayer->getWidth(),
                         remotePlayer->getHeight());
    }
}

void GameEngine::movePlayerLeft()
{
    targetVelocityX = -0.5;
}

void GameEngine::movePlayerRight()
{
    targetVelocityX = 0.5;
}

void GameEngine::stopPlayerMovement()
{
    targetVelocityX = 0;
}

void GameEngine::jumpPlayer()
{
    if (player->isOnPlatform() && !player->isJumping()) {
        double jumpVelocity = -10 * deltaTime * 60;

        
        if (bounceActive) {
            jumpVelocity *= 1.5;
        }

        player->jump(jumpVelocity);
    }
}

void GameEngine::checkCollisions()
{
    
    for (Platform *platform : platforms) {
        int platformScreenY = platform->getY() - scrollOffset;
        int platformTop = platformScreenY;

        
        int playerBottomScreen = player->getY() - scrollOffset + player->getHeight() / 2;

        
        if (player->getVelocity() >= 0 &&
            playerBottomScreen >= platformTop - 2 &&
            playerBottomScreen <= platformTop + 12 &&
            player->getX() + player->getWidth() / 2 - 2 > platform->getX() &&
            player->getX() - player->getWidth() / 2 + 2 < platform->getX() + platform->getWidth()) {

            
            player->setY(platformTop + scrollOffset - player->getHeight() / 2);
            player->setVelocity(0);
            player->setJumping(false);
            player->setOnPlatform(true);

            if (!platform->isPassed()) {
                platform->setPassed(true);
                score += 10;
            }

            
            switch(platform->getType()) {
            case Platform::BOOST:
                boostActive = true;
                boostTimer = 200;
                break;
            case Platform::SLOW:
                slowActive = true;
                slowTimer = 200;
                break;
            case Platform::BOUNCE:
                bounceActive = true;
                bounceTimer = 200;
                break;
            default:
                
                break;
            }

            break;
        }
    }

    
    for (Platform *platform : platformBatch) {
        int platformScreenY = platform->getY() - scrollOffset;
        int platformTop = platformScreenY;

        
        int playerBottomScreen = player->getY() - scrollOffset + player->getHeight() / 2;

        
        if (player->getVelocity() >= 0 &&
            playerBottomScreen >= platformTop - 2 &&
            playerBottomScreen <= platformTop + 12 &&
            player->getX() + player->getWidth() / 2 - 2 > platform->getX() &&
            player->getX() - player->getWidth() / 2 + 2 < platform->getX() + platform->getWidth()) {

            
            player->setY(platformTop + scrollOffset - player->getHeight() / 2);
            player->setVelocity(0);
            player->setJumping(false);
            player->setOnPlatform(true);

            if (!platform->isPassed()) {
                platform->setPassed(true);
                score += 10;
            }

            
            switch(platform->getType()) {
            case Platform::BOOST:
                boostActive = true;
                boostTimer = 200;
                break;
            case Platform::SLOW:
                slowActive = true;
                slowTimer = 200;
                break;
            case Platform::BOUNCE:
                bounceActive = true;
                bounceTimer = 200;
                break;
            default:
                
                break;
            }

            
            platforms.append(platform);
            platformBatch.removeOne(platform);
            break;
        }
    }
}

void GameEngine::removeOffscreenPlatforms()
{
    
    for (int i = platforms.size() - 1; i >= 0; i--) {
        Platform *platform = platforms[i];
        int platformScreenY = platform->getY() - scrollOffset;

        
        if (platformScreenY < -platformHeight - 50) {
            platforms.removeAt(i);
            delete platform;
        }
    }

    
    for (int i = platformBatch.size() - 1; i >= 0; i--) {
        Platform *platform = platformBatch[i];
        int platformScreenY = platform->getY() - scrollOffset;

        
        if (platformScreenY < -platformHeight - 50) {
            platformBatch.removeAt(i);
            delete platform;
        }
    }
}

void GameEngine::checkGameOver()
{
    
    int playerScreenY = player->getY() - scrollOffset;
    if (playerScreenY > height + 10) {
        gameOver = true;
        return;
    }

    
    if (playerScreenY < -50) {
        gameOver = true;
        return;
    }
}

void GameEngine::onPlayerPositionReceived(double x, double y, double velocity)
{
    
    if (!remotePlayer) {
        remotePlayer = new Player(x, y, 24, 30);
    } else {
        remotePlayer->setX(x);
        remotePlayer->setY(y);
        remotePlayer->setVelocity(velocity);
    }
}


void GameEngine::onGameStateReceived(const QByteArray &state)
{
    
    Q_UNUSED(state);
    
}

void GameEngine::onScoreUpdateReceived(int score)
{
    
    Q_UNUSED(score);
    
}


void GameEngine::setNetwork(Network* network, bool isHost)
{
    
    if (this->network) {
        disconnect(this->network, &Network::playerPositionReceived,
                   this, &GameEngine::onPlayerPositionReceived);
        disconnect(this->network, &Network::gameStateReceived,
                   this, &GameEngine::onGameStateReceived);
        disconnect(this->network, &Network::scoreUpdateReceived,
                   this, &GameEngine::onScoreUpdateReceived);
        disconnect(this->network, &Network::platformDataReceived,
                   this, &GameEngine::onPlatformDataReceived);
        disconnect(this->network, &Network::gameInitDataReceived,
                   this, &GameEngine::onGameInitDataReceived);
    }

    this->network = network;
    this->isMultiplayerMode = true; 
    this->isHost = isHost; 

    
    if (isMultiplayerMode && !remotePlayer) {
        remotePlayer = new Player(width / 2 - 50, height / 2, 24, 30);
    }

    
    if (network) {
        connect(network, &Network::playerPositionReceived,
                this, &GameEngine::onPlayerPositionReceived);
        connect(network, &Network::gameStateReceived,
                this, &GameEngine::onGameStateReceived);
        connect(network, &Network::scoreUpdateReceived,
                this, &GameEngine::onScoreUpdateReceived);
        connect(network, &Network::platformDataReceived,
                this, &GameEngine::onPlatformDataReceived);
        connect(network, &Network::gameInitDataReceived,
                this, &GameEngine::onGameInitDataReceived);
    }
}

bool GameEngine::isGameOver() const { return gameOver; }
int GameEngine::getScore() const { return score; }
int GameEngine::getScrollOffset() const { return scrollOffset; }
double GameEngine::getPlayerVelocity() const { return player->getVelocity(); }
bool GameEngine::isPlayerOnPlatform() const { return player->isOnPlatform(); }
int GameEngine::getPlatformCount() const { return platforms.size() + platformBatch.size(); }
double GameEngine::getPlayerY() const { return player->getY(); }
int GameEngine::getPlayerScreenY() const { return player->getY() - scrollOffset; }
bool GameEngine::isBoostActive() const { return boostActive; }
bool GameEngine::isSlowActive() const { return slowActive; }
bool GameEngine::isBounceActive() const { return bounceActive; }
int GameEngine::getGameTime() const { return gameTime; }
