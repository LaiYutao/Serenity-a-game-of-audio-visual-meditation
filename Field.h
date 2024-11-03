#ifndef FIELD_H
#define FIELD_H
#include"Point.h"
#include"ScreenManager.h"
#include"Medium.h"
#include"RayTip.h"
#include<chrono>
#include<cmath>

class Field 
{
public:
	Field(const double& creationTime, const Point& s_Position, const double& s_Amplitude, const double& s_Frequency, const double& s_InitialPhase, const double& s_Speed);
	virtual ~Field() {}; //对于刚开始振动的点（包括波源，和首次被激发的Medium）都是从0开始震动的，因此只需得到创建/触发时间，倒退初相位即可；
	                     //Field的初相位参量，指的是波源处的点，在t=0时刻下，振动方程的初相位，只能为π/2（下一刻向下振动），或者-π/2（下一刻向上振动）；
	
	virtual void ActivateMedium(const double& timeOfNow, const double& frameTime) = 0;//向四面八方发送“激活射线”
	void ClearIfActivated();

	double getCreationTime()const;
	std::vector<Medium>* getPtrMediumLayer();
	Point getSourcePosition()const;
	double getSourceAmplitude() const;
	double getSourceFrequency() const;
	double getSourceInitialPhase() const;
	double getSourceSpeed() const;
	void setSourcePosition(const Point& sourcePosition);
	void setSourceAmplitude(const double& s_Amplitude);
	void setSourceFrequency(const double &s_Frequency);
	void setSourceInitialPhase(const double& s_InitialPhase);
	void setSourceSpeed(const int& s_Speed);

protected:
	bool StopActivation;
	static const int NumberOfRay = ((ScreenWidth + ScreenHeight) * 2 - 4)*16;//最外圈点的个数的倍数，作为发出光线的数量（4倍时已经大致可行，8倍时中间区域只剩少数，12倍时多数情况下只剩边界有一些毛刺,16倍时边界仍存在一些毛刺）
	bool IfActivated[ScreenWidth * ScreenHeight];
	RayTip BunchOfRayTips[NumberOfRay];
	std::vector<Medium> MediumLayer;
private:
	double CreationTime;
	Point SourcePosition;
	double SourceInitialPhase;
	double SourceFrequency;
	double SourceAmplitude;
	double SourceSpeed;
};
#endif // FIELD_H
