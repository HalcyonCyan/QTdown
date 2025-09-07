#include "player.h"

Player::Player(double x, double y, double width, double height)
    : x(x), y(y), width(width), height(height), velocity(0), jumping(false), onPlatform(false)
{
}

void Player::reset(double x, double y)
{
    this->x = x;
    this->y = y;
    velocity = 0;
    jumping = false;
    onPlatform = true; // 初始时玩家站在平台上
}

void Player::draw(QPainter &painter, int scrollOffset) const
{
    painter.setBrush(Qt::blue);
    painter.setPen(Qt::darkBlue);
    int playerScreenY = y - scrollOffset;
    painter.drawRect(x - width/2, playerScreenY - height/2, width, height);
}

void Player::move(double dx, double dy, int screenWidth)
{
    x += dx;
    y += dy;

    // 确保玩家不会移出屏幕左右边界
    if (x < width/2) x = width/2;
    if (x > screenWidth - width/2) x = screenWidth - width/2;
}

void Player::jump(double velocity)
{
    this->velocity = velocity;
    jumping = true;
    onPlatform = false;
}

void Player::applyGravity(double gravity)
{
    if (!onPlatform) {
        velocity += gravity;
        y += velocity;
    }
}
// Getter 方法
double Player::getX() const { return x; }
double Player::getY() const { return y; }
double Player::getWidth() const { return width; }
double Player::getHeight() const { return height; }
double Player::getVelocity() const { return velocity; }
bool Player::isJumping() const { return jumping; }
bool Player::isOnPlatform() const { return onPlatform; }

// Setter 方法
void Player::setX(double x) { this->x = x; }
void Player::setY(double y) { this->y = y; }
void Player::setVelocity(double velocity) { this->velocity = velocity; }
void Player::setJumping(bool jumping) { this->jumping = jumping; }
void Player::setOnPlatform(bool onPlatform) { this->onPlatform = onPlatform; }
