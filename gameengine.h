#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QObject>
#include <QList>
#include <QVector>
#include <QDateTime>
#include <QPainter>
#include "player.h"
#include "platform.h"
#include "network.h"  // 添加网络头文件

class GameEngine : public QObject
{
    Q_OBJECT
public:
    // 修改构造函数，添加网络参数
    explicit GameEngine(int width, int height, bool isMultiplayer = false,
                        Network* network = nullptr, QObject *parent = nullptr);



    ~GameEngine();

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
    bool isSlowActive() const;      // 是否处于减速状态
    bool isBounceActive() const;    // 是否处于高跳状态
    int getGameTime() const;        // 获取游戏时间

    // 添加远程玩家相关方法
    Player* getRemotePlayer() const { return remotePlayer; }
    bool isMultiplayer() const { return isMultiplayerMode; }
    void setNetwork(Network* network, bool isHost);

signals:
    void scoreUpdated(int score);

private:
    void generateInitialPlatforms();
    void generatePlatformBatch();
    void addPlatformToBatch(int y);
    void checkCollisions();
    void removeOffscreenPlatforms();
    void checkGameOver();
    void updateHorizontalMovement();

    // 添加碰撞检测方法
    void checkPlayerCollisions();

    int width;
    int height;
    Player *player;
    // 添加远程玩家
    Player* remotePlayer;
    QList<Platform*> platforms;
    QList<Platform*> platformBatch;
    QVector<int> recentPlatformXs;
    int score;
    int scrollOffset;
    bool gameOver;

    // 平台参数
    int platformWidth;
    int platformHeight;
    int platformMinGap;
    int platformMaxGap;
    int platformScrollSpeed;

    // 批次生成参数
    int batchSize;
    int batchThreshold;
    const int recentPlatformCount = 5; // 记录最近平台位置的数量

    // 特殊状态
    bool boostActive;
    int boostTimer;
    bool slowActive;     // 减速状态
    int slowTimer;       // 减速计时器
    bool bounceActive;   // 高跳状态
    int bounceTimer;     // 高跳计时器

    // 平滑移动相关变量
    qint64 initTime;
    qint64 lastUpdateTime;
    double deltaTime;
    float targetVelocityX;
    float currentVelocityX;
    float bei;
    const float acceleration = 1.1f;
    const float deceleration = 0.7f;
    const float maxNormalSpeed = 6.0f;    // 正常最大速度
    const float maxBoostSpeed = 10.0f;     // 加速最大速度
    int gameTime;

    // 添加网络相关成员
    Network* network;
    bool isMultiplayerMode;

    // 添加远程玩家位置同步计时器
    qint64 lastPositionUpdateTime;
    const qint64 positionUpdateInterval = 6; // 每6ms更新一次位置

public slots:
    void onPlayerPositionReceived(double x, double y, double velocity);
    void onGameStateReceived(const QByteArray &state);
    void onScoreUpdateReceived(int score);
};

#endif // GAMEENGINE_H
