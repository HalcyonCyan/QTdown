#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QObject>
#include <QList>
#include <QVector>
#include <QDateTime>
#include <QPainter>
#include "player.h"
#include "platform.h"
#include "network.h"

class GameEngine : public QObject
{
    Q_OBJECT
public:
    explicit GameEngine(int width, int height, bool isMultiplayer = false,
                        Network* network = nullptr, QObject *parent = nullptr);
    ~GameEngine();

    struct FloatingObject {
        QPointF position;
        bool active;
        Platform* parentPlatform;
        float radius;
    };

    void initGame();
    void update();
    void draw(QPainter &painter, bool showDebugInfo = false);

    void movePlayerLeft();
    void movePlayerRight();
    void stopPlayerMovement();
    void jumpPlayer();

    bool isGameOver() const;
    int getScore() const;
    int getScrollOffset() const;
    double getPlayerVelocity() const;
    bool isPlayerOnPlatform() const;
    int getPlatformCount() const;
    double getPlayerY() const;
    int getPlayerScreenY() const;
    bool isBoostActive() const;
    bool isSlowActive() const;
    bool isBounceActive() const;
    int getGameTime() const;
    bool isWaitingForInitialData() const { return waitingForInitialData; }
    
    Player* getRemotePlayer() const { return remotePlayer; }
    bool isMultiplayer() const { return isMultiplayerMode; }
    void setNetwork(Network* network, bool isHost);

    
    bool checkPlayerCollision() const;

signals:
    void scoreUpdated(int score);

private:

    bool isHost; 
    bool waitingForInitialData;
    void serializePlatforms(QByteArray& data) const;
    void deserializePlatforms(const QByteArray& data);
    void serializeGameState(QByteArray& data) const;
    void deserializeGameState(const QByteArray& data);
    void generateInitialPlatforms();
    void generatePlatformBatch();
    void addPlatformToBatch(int y);
    void checkCollisions();
    void removeOffscreenPlatforms();
    void checkGameOver();
    void updateHorizontalMovement();
    QVector<FloatingObject> floatingObjects;

    int width;
    int height;
    Player *player;
    Player* remotePlayer; 
    QList<Platform*> platforms;
    QList<Platform*> platformBatch;
    QVector<int> recentPlatformXs;
    int score;
    int scrollOffset;
    bool gameOver;

    
    int platformWidth;
    int platformHeight;
    int platformMinGap;
    int platformMaxGap;
    int platformScrollSpeed;

    
    int batchSize;
    int batchThreshold;
    const int recentPlatformCount = 5;

    
    bool boostActive;
    int boostTimer;
    bool slowActive;
    int slowTimer;
    bool bounceActive;
    int bounceTimer;

    
    qint64 initTime;
    qint64 lastUpdateTime;
    double deltaTime;
    float targetVelocityX;
    float currentVelocityX;
    float bei;
    const float acceleration = 1.1f;
    const float deceleration = 0.7f;
    const float maxNormalSpeed = 6.0f;
    const float maxBoostSpeed = 10.0f;
    int gameTime;

    
    Network* network;
    bool isMultiplayerMode;

    
    qint64 lastPositionUpdateTime;
    const qint64 positionUpdateInterval = 100; 

public slots:
    void onPlayerPositionReceived(double x, double y, double velocity);
    void onGameStateReceived(const QByteArray &state);
    void onScoreUpdateReceived(int score);
    void onPlatformDataReceived(const QByteArray &data);   
    void onGameInitDataReceived(const QByteArray &data);
};

#endif 
