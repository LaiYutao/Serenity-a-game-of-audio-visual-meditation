#include "Medium.h"

Medium::Medium(const double& sourceCreationTime):SourceCreationTime(sourceCreationTime), CurrentAmplitude(0), CurrentFrequency(0), CurrentInitialPhase(0), Height(0)
{
}

//介质被激活，接受场源的参数
void Medium::GetActivated(const double& timeOfNow, const double& newAmplitude, const double& newFrequency, const double& newInitialPhase)
{
    //计算并设置振动方程参数（t=timeOfNow时的情况）  
    //振幅、频率直接传入即可  
    CurrentAmplitude = newAmplitude;   
    CurrentFrequency = newFrequency;

    //t=timeOfNow时，Height为0，结合振动方向（向上还是向下，此时相位就是Source处初相位），推出以t=0为起点的初相位
    CurrentInitialPhase = newInitialPhase - 2*PI*CurrentFrequency * (timeOfNow - SourceCreationTime); // t=timeOfNow时，cos内部对应就是newInitialPhase; 角频率ω=2πf（frequency）

}

//振动，即是改变Height的值，后续由Monitor对象转化为字符显示
void Medium::Vibrate(const double& timeOfNow)
{
    if (CurrentAmplitude == 0)
    {
        return;//如果CurrentAmplitude为零，即为不振动
    }
    else
    {
        Height = CurrentAmplitude * cos(2 * PI * CurrentFrequency * timeOfNow + CurrentInitialPhase);
    }
}

double Medium::getCurrentAmplitude() const
{
    return CurrentAmplitude;
}

double Medium::getCurrentFrequency() const 
{
    return CurrentFrequency;
}

double Medium::getCurrentInitialPhase() const 
{
    return CurrentInitialPhase;
}

double Medium::getHeight()const
{
    return Height;
}