#ifndef RAYTIP_H
#define RAYTIP_H
#include"Point.h"

class RayTip : public Point 
{
public:
	RayTip();//用于创建RayTip数组
	RayTip(double xPos, double yPos,double directionAngle, double distanceToSource=0);
	virtual ~RayTip() {};
	
	double getDirectionAngle()const;
	double getDistanceToSource()const;
	void setDirectionAngle(const double& directionAngle);
	void setDistanceToSource(const double& distanceToSource);

private:
	double DirectionAngle;
	double DistanceToSource;
};
#endif // RAYTIP_H
