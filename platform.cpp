
#include "platform.h"

Platform::Platform(int x, int y, int width, int height, bool passed, PlatformType type)
    : x(x), y(y), width(width), height(height), passed(passed), type(type)
{
}

void Platform::draw(QPainter &painter, int scrollOffset) const
{
    int platformScreenY = y - scrollOffset;

    
    switch(type) {
    case BOOST:
        painter.setBrush(Qt::red);
        break;
    case SLOW:
        painter.setBrush(Qt::blue);
        break;
    case BOUNCE:
        painter.setBrush(Qt::green);
        break;
    default:
        painter.setBrush(QColor(100, 200, 100)); 
        break;
    }

    painter.setPen(Qt::black);
    painter.drawRect(x, platformScreenY, width, height);
}


int Platform::getX() const { return x; }
int Platform::getY() const { return y; }
int Platform::getWidth() const { return width; }
int Platform::getHeight() const { return height; }
bool Platform::isPassed() const { return passed; }
Platform::PlatformType Platform::getType() const { return type; }


void Platform::setPassed(bool passed) { this->passed = passed; }
