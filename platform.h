#ifndef PLATFORM_H
#define PLATFORM_H

#include <QPainter>

class Platform
{
public:
    enum PlatformType {
        NORMAL,
        BOOST  // 红色加速平台
    };

    Platform(int x, int y, int width, int height, bool passed = false, PlatformType type = NORMAL);

    void draw(QPainter &painter, int scrollOffset) const;

    // Getter 方法
    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;
    bool isPassed() const;
    PlatformType getType() const;

    // Setter 方法
    void setPassed(bool passed);

private:
    int x;
    int y;
    int width;
    int height;
    bool passed;
    PlatformType type;
};

#endif // PLATFORM_H
