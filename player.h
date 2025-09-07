#ifndef PLAYER_H
#define PLAYER_H

#include <QPainter>

class Player
{
public:
    Player(double x, double y, double width, double height);

    void reset(double x, double y);
    void draw(QPainter &painter, int scrollOffset) const;
    void move(double dx, double dy, int screenWidth);
    void jump(double velocity);
    void applyGravity(double gravity);

    // Getter 方法
    double getX() const;
    double getY() const;
    double getWidth() const;
    double getHeight() const;
    double getVelocity() const;
    bool isJumping() const;
    bool isOnPlatform() const;

    // Setter 方法
    void setX(double x);
    void setY(double y);
    void setVelocity(double velocity);
    void setJumping(bool jumping);
    void setOnPlatform(bool onPlatform);

private:
    double x;
    double y;
    double width;
    double height;
    double velocity;
    bool jumping;
    bool onPlatform;
};

#endif // PLAYER_H
