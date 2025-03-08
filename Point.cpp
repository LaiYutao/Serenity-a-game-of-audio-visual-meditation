#include"Point.h"
Point::Point(double xPos, double yPos) : XPos(xPos), YPos(yPos) {}

double Point::getXPos() const
{
    return XPos;
}

double Point::getYPos() const
{
    return YPos;
}

void Point::setXPos(double xPos)
{
    XPos = xPos;
}

void Point::setYPos(double yPos)
{
    YPos = yPos;
}
