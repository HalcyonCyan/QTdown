// gameengine.h
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QObject>
#include <QList>
#include <QVector>
#include <QDateTime>
#include "player.h"
#include "platform.h"

class GameEngine : public QObject
{
    Q_OBJECT
public:
    explicit GameEngine(int width, int height, QObject *parent = nullptr);
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
    bool isSlowActive() const;      // 新增：是否处于减速状态
    bool isBounceActive() const;    // 新增：是否处于高跳状态
    int getGameTime() const; // 新增：获取游戏时间

private:
    void generateInitialPlatforms();
    void generatePlatformBatch();
    void addPlatformToBatch(int y);
    void checkCollisions();
    void removeOffscreenPlatforms();
    void checkGameOver();
    void updateHorizontalMovement();

    int width;
    int height;
    Player *player;
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
    bool slowActive;     // 新增：减速状态
    int slowTimer;       // 新增：减速计时器
    bool bounceActive;   // 新增：高跳状态
    int bounceTimer;     // 新增：高跳计时器

    // 平滑移动相关变量
    qint64 initTime;
    qint64 lastUpdateTime;
    double deltaTime;
    float targetVelocityX;
    float currentVelocityX;
    float bei;
    const float acceleration = 1.1f;
    const float deceleration = 0.7f;
    const float maxNormalSpeed = 6.0f;    // 新增：正常最大速度
    const float maxBoostSpeed = 10.0f;     // 新增：加速最大速度
    int gameTime;           // 新增：游戏进行时间（秒）
};

#endif // GAMEENGINE_H
