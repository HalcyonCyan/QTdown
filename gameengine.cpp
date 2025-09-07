#include "gameengine.h"
#include <QDebug>
#include <QRandomGenerator>
#include <algorithm>
#include <climits>
#include <QPainter>
#include <QDateTime>
#include <cmath>
GameEngine::GameEngine(int width, int height, QObject *parent)
    : QObject(parent), width(400), height(500), score(0), scrollOffset(0), gameOver(false),
    boostActive(false), boostTimer(0), initTime(0), lastUpdateTime(0), deltaTime(0),
    targetVelocityX(0), currentVelocityX(0), gameTime(0)
{
    player = new Player(width / 2, height / 2, 24, 30);

    platformWidth = 90;
    platformHeight = 14;
    platformMinGap = 35;
    platformMaxGap = 90;
    platformScrollSpeed = 1;

    // 批次生成参数
    batchSize = 10;      // 每批生成10个砖块
    batchThreshold = 7;  // 当7个砖块进入屏幕时生成新批次

    initGame();
}

void GameEngine::initGame()
{
    // 重置玩家位置到屏幕中央
    player->reset(width / 2, height / 2);

    qDeleteAll(platforms);
    platforms.clear();
    qDeleteAll(platformBatch);
    platformBatch.clear();
    recentPlatformXs.clear();

    generateInitialPlatforms();

    // 生成第一批待进入的砖块
    generatePlatformBatch();

    score = 0;
    scrollOffset = 0;
    gameOver = false;
    boostActive = false;
    boostTimer = 0;

    // 重置移动状态
    targetVelocityX = 0;
    currentVelocityX = 0;

    // 设置游戏开始时间
    initTime = QDateTime::currentMSecsSinceEpoch();
    lastUpdateTime = initTime;
    gameTime = 0;
}

GameEngine::~GameEngine()
{
    delete player;
    qDeleteAll(platforms);
    platforms.clear();
    qDeleteAll(platformBatch);
    platformBatch.clear();
}

void GameEngine::update()
{
    // 计算时间增量
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    if (lastUpdateTime == 0) {
        lastUpdateTime = currentTime;
    }
    deltaTime = (currentTime - lastUpdateTime) / 1000.0; // 转换为秒
    lastUpdateTime = currentTime;

    // 计算游戏进行时间（ms）
    gameTime = static_cast<int>((currentTime - initTime) );
    qDebug()<<"gametime"<<gameTime;
    // 限制deltaTime以避免极端情况
    if (deltaTime > 0.1) {
        deltaTime = 0.1;
    }

    // 更新水平移动
    updateHorizontalMovement();

    // 更新加速状态计时器
    if (boostActive) {
        boostTimer -= deltaTime * 60;
        if (boostTimer <= 0) {
            boostActive = false;
        }
    }
    float bei=std::min(1.0+gameTime*1.0/100000,1.5);
    qDebug()<<"bei"<<bei<<"gra"<<0.33 * deltaTime * 60*bei;
    // 保存上一帧的平台状态
    bool wasOnPlatform = player->isOnPlatform();
    player->setOnPlatform(false);

    // 应用重力
    player->applyGravity(0.33 * deltaTime * 60*bei);

    // 平台自动上升
    scrollOffset += platformScrollSpeed * deltaTime * 220*bei;

    // 碰撞检测
    checkCollisions();

    // 如果之前站在平台上但现在不在了，恢复重力
    if (wasOnPlatform && !player->isOnPlatform()) {
        player->setJumping(true);
    }

    // 检查批次生成条件 - 基于最高平台位置
    int highestPlatformY = 0;
    for (Platform *platform : platforms) {
        if (platform->getY() > highestPlatformY) {
            highestPlatformY = platform->getY();
        }
    }
    for (Platform *platform : platformBatch) {
        if (platform->getY() > highestPlatformY) {
            highestPlatformY = platform->getY();
        }
    }

    // 如果最高平台的位置接近屏幕底部，生成新批次
    int highestScreenY = highestPlatformY - scrollOffset;
    if (highestScreenY < height + 200) { // 提前200像素生成
        generatePlatformBatch();
    }

    // 将进入屏幕的砖块从批次移动到主列表
    for (int i = platformBatch.size() - 1; i >= 0; i--) {
        Platform *platform = platformBatch[i];
        int platformScreenY = platform->getY() - scrollOffset;
        if (platformScreenY < height) {
            platforms.append(platform);
            platformBatch.removeAt(i);
        }
    }

    // 移除屏幕外的平台
    removeOffscreenPlatforms();

    // 检查游戏结束条件
    checkGameOver();
}


void GameEngine::updateHorizontalMovement()
{
    // 平滑加速和减速
    if (targetVelocityX != 0) {
        // 加速
        if (currentVelocityX * targetVelocityX >= 0) {
            // 同方向加速
            currentVelocityX += targetVelocityX * acceleration * deltaTime * 60;

            // 限制最大速度
            float maxSpeed = boostActive ? maxBoostSpeed : maxNormalSpeed;
            if (fabs(currentVelocityX) > maxSpeed) {
                currentVelocityX = (currentVelocityX > 0) ? maxSpeed : -maxSpeed;
            }
        } else {
            // 反方向减速再加速
            currentVelocityX += targetVelocityX * acceleration * 2 * deltaTime * 60;
        }
    } else {
        // 减速到停止
        if (currentVelocityX > 0) {
            currentVelocityX -= deceleration * deltaTime * 60;
            if (currentVelocityX < 0) currentVelocityX = 0;
        } else if (currentVelocityX < 0) {
            currentVelocityX += deceleration * deltaTime * 60;
            if (currentVelocityX > 0) currentVelocityX = 0;
        }
    }

    // 应用水平移动
    if (fabs(currentVelocityX) > 0.1f) {
        player->move(currentVelocityX * deltaTime * 60, 0, width);
    }
}
void GameEngine::generateInitialPlatforms()
{
    // 在玩家脚下生成一个平台
    Platform *startingPlatform = new Platform(
        player->getX() - platformWidth / 2,
        player->getY() + player->getHeight() / 2,
        platformWidth,
        platformHeight,
        true
        );
    platforms.append(startingPlatform);
    recentPlatformXs.append(startingPlatform->getX());

    // 生成其他初始平台
    int currentY = player->getY() + player->getHeight() / 2 + platformMinGap;
    for (int i = 0; i < 11; i++) { // 总共11个平台（包括起始平台）
        addPlatformToBatch(currentY);
        currentY += QRandomGenerator::global()->bounded(platformMinGap, platformMaxGap);
    }
}

void GameEngine::generatePlatformBatch()
{
    // 找到最高的平台Y坐标
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

    // 生成新批次的砖块
    int currentY = highestY + QRandomGenerator::global()->bounded(platformMinGap, platformMaxGap);
    for (int i = 0; i < batchSize; i++) {
        addPlatformToBatch(currentY);
        currentY += QRandomGenerator::global()->bounded(platformMinGap, platformMaxGap);
    }
}

void GameEngine::addPlatformToBatch(int y)
{
    int newX;
    int platformWidthToUse = platformWidth;

    // 计算最近平台的平均x位置
    int averageX = 0;
    if (!recentPlatformXs.isEmpty()) {
        for (int x : recentPlatformXs) {
            averageX += x;
        }
        averageX /= recentPlatformXs.size();
    } else {
        averageX = width / 2;
    }

    // 决定平台类型（红色平台概率）
    int redProbability = std::min(score / 80, 10);
    Platform::PlatformType type = (QRandomGenerator::global()->bounded(100) < redProbability) ?
                                      Platform::BOOST : Platform::NORMAL;

    // 如果是红色平台，宽度略窄
    if (type == Platform::BOOST) {
        platformWidthToUse = 60+QRandomGenerator::global()->bounded(100)/10;
    }

    // 决定是否生成两个并排的平台（20%概率）
    if (QRandomGenerator::global()->bounded(100) < 20) {
        // 生成两个宽度为50的平台
        int gap=std::abs(averageX-200)/2+3 ;
        qDebug()<<"gap"<<gap;
        // 根据平均位置决定倾向哪一侧
        bool generateOnRight = averageX < 200;

        if (generateOnRight) {
            // 倾向于右侧
            if(QRandomGenerator::global()->bounded(100) < 60)
                gap=0;
            int x1 = QRandomGenerator::global()->bounded(width/2 - 50 - gap);
            int x2 = width/2 + gap + QRandomGenerator::global()->bounded(width/2 - 50 - gap);

            // 确保不超出屏幕
            x1 = std::max(0, std::min(x1, width - 50));
            x2 = std::max(0, std::min(x2, width - 50));

            Platform *platform1 = new Platform(x1, y, 50, platformHeight, false, type);
            Platform *platform2 = new Platform(x2, y, 50, platformHeight, false, type);

            platformBatch.append(platform1);
            platformBatch.append(platform2);

            // 记录位置（使用两个平台的平均位置）
            recentPlatformXs.append((x1 + x2) / 2);
        } else {
            // 倾向于左侧
            if(QRandomGenerator::global()->bounded(100) < 40)
                gap=0;
            int x1 = QRandomGenerator::global()->bounded(width/2 - 50 - gap);
            int x2 = width/2 + gap + QRandomGenerator::global()->bounded(width/2 - 50 - gap);

            // 确保不超出屏幕
            x1 = std::max(0, std::min(x1, width - 50));
            x2 = std::max(0, std::min(x2, width - 50));

            Platform *platform1 = new Platform(x1, y, 50, platformHeight, false, type);
            Platform *platform2 = new Platform(x2, y, 50, platformHeight, false, type);

            platformBatch.append(platform1);
            platformBatch.append(platform2);

            // 记录位置（使用两个平台的平均位置）
            recentPlatformXs.append((x1 + x2) / 2);
        }
    } else {
        // 生成单个平台

        // 根据平均位置决定倾向哪一侧
        bool generateOnRight = averageX < width / 2;

        if (generateOnRight) {
            // 倾向于右侧（屏幕右半部分）
            newX = width/2 + QRandomGenerator::global()->bounded(width/2 - platformWidthToUse - 20) +25;
        } else {
            // 倾向于左侧（屏幕左半部分）
            newX = QRandomGenerator::global()->bounded(width/2 - platformWidthToUse - 20) + 10;
        }

        // 确保不超出屏幕
        newX = std::max(0, std::min(newX, width - platformWidthToUse));

        Platform *platform = new Platform(newX, y, platformWidthToUse, platformHeight, false, type);
        platformBatch.append(platform);

        // 记录位置
        recentPlatformXs.append(newX);
    }

    // 保持最近平台记录的数量不超过设定值
    if (recentPlatformXs.size() > recentPlatformCount) {
        recentPlatformXs.removeFirst();
    }

    qDebug() << "添加到批次，Y坐标:" << y << "屏幕Y:" << (y - scrollOffset)
             << "平均X:" << averageX << "倾向:" << (averageX < width / 2 ? "右侧" : "左侧")
             << "偏移" << scrollOffset;
}

void GameEngine::draw(QPainter &painter, bool showDebugInfo)
{
    // 首先绘制已进入屏幕的平台（主列表中的平台）
    painter.setPen(Qt::black);
    for (Platform *platform : platforms) {
        int platformScreenY = platform->getY() - scrollOffset;
        if (platformScreenY > -platformHeight && platformScreenY < height) {
            // 正常绘制已进入屏幕的平台
            if (platform->getType() == Platform::BOOST) {
                painter.setBrush(Qt::red);
            } else {
                painter.setBrush(QColor(100, 200, 100));
            }
            painter.drawRect(platform->getX(), platformScreenY, platform->getWidth(), platform->getHeight());

            // 调试：显示平台碰撞区域
            if (showDebugInfo) {
                painter.setPen(Qt::red);
                painter.drawLine(platform->getX(), platformScreenY,
                                 platform->getX() + platform->getWidth(), platformScreenY);
                painter.setPen(Qt::black);
            }
        }
    }

    // 然后绘制批次中的平台（半透明表示尚未完全进入屏幕）
    for (Platform *platform : platformBatch) {
        int platformScreenY = platform->getY() - scrollOffset;
        if (platformScreenY > -platformHeight && platformScreenY < height) {
            // 半透明绘制批次中的平台
            QColor platformColor = (platform->getType() == Platform::BOOST) ? Qt::red : QColor(100, 200, 100);
            platformColor.setAlpha(128); // 半透明
            painter.setBrush(platformColor);
            painter.setPen(Qt::black);
            painter.drawRect(platform->getX(), platformScreenY, platform->getWidth(), platform->getHeight());

            // 调试：显示平台碰撞区域
            if (showDebugInfo) {
                painter.setPen(Qt::red);
                painter.drawLine(platform->getX(), platformScreenY,
                                 platform->getX() + platform->getWidth(), platformScreenY);
                painter.setPen(Qt::black);
            }
        }
    }

    // 绘制玩家
    player->draw(painter, scrollOffset);

    // 如果加速状态激活，显示特效
    if (boostActive) {
        painter.setBrush(QColor(255, 100, 100, 100));
        painter.setPen(Qt::NoPen);
        int playerScreenY = player->getY() - scrollOffset;
        painter.drawEllipse(player->getX() - player->getWidth()/2 - 5,
                            playerScreenY - player->getHeight()/2 - 5,
                            player->getWidth() + 10,
                            player->getHeight() + 10);
    }
}

void GameEngine::movePlayerLeft()
{
    targetVelocityX = boostActive ? -1.2 : -0.5;
    qDebug()<<targetVelocityX;
}

void GameEngine::movePlayerRight()
{
    targetVelocityX = boostActive ? 1.2 : 0.5;
    qDebug()<<targetVelocityX;
}

void GameEngine::stopPlayerMovement()
{
    targetVelocityX = 0;
}

void GameEngine::jumpPlayer()
{
    if (player->isOnPlatform() && !player->isJumping()) {
        player->jump(-12 * deltaTime * 60);
    }
}

void GameEngine::checkCollisions()
{
    // 检查与已进入屏幕的平台的碰撞
    for (Platform *platform : platforms) {
        int platformScreenY = platform->getY() - scrollOffset;
        int platformTop = platformScreenY;

        // 计算玩家底部位置（屏幕坐标）
        int playerBottomScreen = player->getY() - scrollOffset + player->getHeight()/2;

        // 碰撞检测条件
        if (player->getVelocity() >= 0 &&
            playerBottomScreen >= platformTop - 2 &&
            playerBottomScreen <= platformTop + 12 &&
            player->getX() + player->getWidth()/2 - 2 > platform->getX() &&
            player->getX() - player->getWidth()/2 + 2 < platform->getX() + platform->getWidth()) {

            // 关键修复：玩家站在平台上
            player->setY(platformTop + scrollOffset - player->getHeight()/2);
            player->setVelocity(0);
            player->setJumping(false);
            player->setOnPlatform(true);

            if (!platform->isPassed()) {
                platform->setPassed(true);
                score += 10;
            }

            // 检查是否是加速平台
            if (platform->getType() == Platform::BOOST) {
                boostActive = true;
                boostTimer = 80;
            }

            break;
        }
    }

    // 检查与批次中平台的碰撞（如果它们已经进入屏幕但尚未移动到主列表）
    for (Platform *platform : platformBatch) {
        int platformScreenY = platform->getY() - scrollOffset;
        int platformTop = platformScreenY;

        // 计算玩家底部位置（屏幕坐标）
        int playerBottomScreen = player->getY() - scrollOffset + player->getHeight()/2;

        // 碰撞检测条件
        if (player->getVelocity() >= 0 &&
            playerBottomScreen >= platformTop - 2 &&
            playerBottomScreen <= platformTop + 12 &&
            player->getX() + player->getWidth()/2 - 2 > platform->getX() &&
            player->getX() - player->getWidth()/2 + 2 < platform->getX() + platform->getWidth()) {

            // 关键修复：玩家站在平台上
            player->setY(platformTop + scrollOffset - player->getHeight()/2);
            player->setVelocity(0);
            player->setJumping(false);
            player->setOnPlatform(true);

            if (!platform->isPassed()) {
                platform->setPassed(true);
                score += 10;
            }

            // 检查是否是加速平台
            if (platform->getType() == Platform::BOOST) {
                boostActive = true;
                boostTimer = 300;
            }

            // 将平台从批次移动到主列表
            platforms.append(platform);
            platformBatch.removeOne(platform);
            break;
        }
    }
}

void GameEngine::removeOffscreenPlatforms()
{
    // 移除已离开屏幕顶部的平台（只移除已经完全离开屏幕的平台）
    for (int i = platforms.size() - 1; i >= 0; i--) {
        Platform *platform = platforms[i];
        int platformScreenY = platform->getY() - scrollOffset;

        // 只有当平台完全离开屏幕顶部（加上一些缓冲）时才删除
        if (platformScreenY < -platformHeight - 50) {
            platforms.removeAt(i);
            delete platform;
        }
    }

    // 移除批次中已离开屏幕顶部的平台（同样只移除完全离开屏幕的）
    for (int i = platformBatch.size() - 1; i >= 0; i--) {
        Platform *platform = platformBatch[i];
        int platformScreenY = platform->getY() - scrollOffset;

        // 只有当平台完全离开屏幕顶部（加上一些缓冲）时才删除
        if (platformScreenY < -platformHeight - 50) {
            platformBatch.removeAt(i);
            delete platform;
        }
    }
}

void GameEngine::checkGameOver()
{
    // 玩家掉出屏幕底部
    int playerScreenY = player->getY() - scrollOffset;
    if (playerScreenY > height+10 ) {
        gameOver = true;
        return;
    }

    // 玩家碰到屏幕顶部
    if (playerScreenY < -50) {
        gameOver = true;
        return;
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
int GameEngine::getGameTime() const { return gameTime; }
