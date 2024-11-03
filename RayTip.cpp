#include "RayTip.h"

RayTip::RayTip() : Point(0, 0), DirectionAngle(0), DistanceToSource(0) 
{
}

RayTip::RayTip(double xPos, double yPos, double directionAngle, double distanceToSource)
    : Point(xPos, yPos), DirectionAngle(directionAngle), DistanceToSource(distanceToSource) 
{
}

double RayTip::getDirectionAngle() const 
{
    return DirectionAngle;
}

double RayTip::getDistanceToSource() const 
{
    return DistanceToSource;
}

void RayTip::setDirectionAngle(const double& directionAngle) 
{
    DirectionAngle = directionAngle;
}

void RayTip::setDistanceToSource(const double& distanceToSource) 
{
    DistanceToSource = distanceToSource;
}