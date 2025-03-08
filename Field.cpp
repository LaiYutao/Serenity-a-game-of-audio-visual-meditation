#include "Field.h"

Field::Field(const double& creationTime, const Point& s_Position,const double& s_Amplitude, const double& s_Frequency, const double& s_InitialPhase, const double& s_Speed)
    : CreationTime(creationTime),SourcePosition(s_Position), SourceAmplitude(s_Amplitude), SourceFrequency(s_Frequency), SourceInitialPhase(s_InitialPhase), SourceSpeed(s_Speed)
{
    StopActivation = false;

    //初始化MediumLayer：MediumLayer中每个Medium接收Field的创建时间;顺便初始化IfActivated
    MediumLayer.resize(ScreenWidth * ScreenHeight);
    for (int i = 0;i < ScreenWidth * ScreenHeight;++i)
    {
        MediumLayer[i] = Medium(this->getCreationTime());
        IfActivated[i] = false;
    }

    //初始化RayTip：以Source坐标为所有RayTip的初始坐标，并分配初始方向，初始距离均为0
    for (int i = 0;i < NumberOfRay;++i)
    {
        double DirectionAngle = i * (2 * PI / NumberOfRay);
        BunchOfRayTips[i] = RayTip(SourcePosition.getXPos(), SourcePosition.getYPos(), DirectionAngle, 0);
    }
}

//将IfActivated和RayTip重置为初始状态
void Field::ClearIfActivated()
{
    StopActivation = false;

    for (int i = 0;i < ScreenWidth * ScreenHeight;++i)
    {
        IfActivated[i] = false;
    }

    for (int i = 0;i < NumberOfRay;++i)
    {
        double DirectionAngle = i * (2 * PI / NumberOfRay);
        BunchOfRayTips[i] = RayTip(SourcePosition.getXPos(), SourcePosition.getYPos(), DirectionAngle, 0);
    }
}

double Field::getCreationTime()const
{
    return CreationTime;
}

std::vector<Medium>* Field::getPtrMediumLayer()
{
    return &MediumLayer;
}

Point Field::getSourcePosition() const 
{
    return SourcePosition;
}

double Field::getSourceAmplitude() const 
{
    return SourceAmplitude;
}

double Field::getSourceFrequency() const 
{
    return SourceFrequency;
}

double Field::getSourceInitialPhase() const 
{
    return SourceInitialPhase;
}

double Field::getSourceSpeed() const 
{
    return SourceSpeed;
}

void Field::setSourcePosition(const Point& sourcePosition) 
{
    SourcePosition = sourcePosition;
}

void Field::setSourceAmplitude(const double& s_Amplitude) 
{
    SourceAmplitude = s_Amplitude;
}

void Field::setSourceFrequency(const double& s_Frequency) 
{
    SourceFrequency = s_Frequency;
}

void Field::setSourceInitialPhase(const double& s_InitialPhase) 
{
    SourceInitialPhase = s_InitialPhase;
}

void Field::setSourceSpeed(const int& s_Speed) 
{
    SourceSpeed = s_Speed;
}