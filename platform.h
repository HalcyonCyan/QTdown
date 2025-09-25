
#ifndef PLATFORM_H
#define PLATFORM_H

#include <QPainter>

class Platform
{
public:
    enum PlatformType {
        NORMAL,
        BOOST,  
        SLOW,   
        BOUNCE  
    };

    Platform(int x, int y, int width, int height, bool passed = false, PlatformType type = NORMAL);

    void draw(QPainter &painter, int scrollOffset) const;

    
    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;
    bool isPassed() const;
    PlatformType getType() const;

    
    void setPassed(bool passed);

private:
    int x;
    int y;
    int width;
    int height;
    bool passed;
    PlatformType type;
};

#endif 
