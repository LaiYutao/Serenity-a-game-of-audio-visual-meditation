#include"CircularField.h"

CircularField::CircularField(const double& creationTime,const Point& s_Position, const double& s_Amplitude, const double& s_Frequency, const double& s_InitialPhase, const double& s_Speed)
	:Field(creationTime,s_Position, s_Amplitude, s_Frequency, s_InitialPhase, s_Speed)
{
}

void CircularField::ActivateMedium(const double& timeOfNow, const double& frameTime)
{
	//如果每个Medium点都已经被激活，就不再进行激活的常规操作
	if (StopActivation == true)return;//只要在一次GameLoop中，Medium全部都已激活，那么只要参数没有改变，后续所有循环中，都不用再激活
	
	bool CheckActivation = true;
	for (int i = 0;i < ScreenWidth * ScreenHeight;++i) 
	{
		if (!IfActivated[i])
		{
			CheckActivation = false;
			break;
		}
	}
	if (CheckActivation == true)
	{
		StopActivation = true;
	}

	//遍历每一条射线
	for (int i = 0;i < NumberOfRay;++i) 
	{
		BunchOfRayTips[i].setDistanceToSource(BunchOfRayTips[i].getDistanceToSource() + this->getSourceSpeed() * frameTime);
		double newXPos = getSourcePosition().getXPos() + BunchOfRayTips[i].getDistanceToSource() * cos(BunchOfRayTips[i].getDirectionAngle());
		BunchOfRayTips[i].setXPos(newXPos);
		double newYPos = getSourcePosition().getYPos() + BunchOfRayTips[i].getDistanceToSource() * sin(BunchOfRayTips[i].getDirectionAngle());
		BunchOfRayTips[i].setYPos(newYPos);
		
		//定位器：判断是否到了可以激活Medium的情形（是否离整数点足够近；而不能单纯看向上还是向下取整）。如果符合条件，就给出判断坐标
		double JudgeRadius = 0.5;
		bool Judger = false;
		double X = BunchOfRayTips[i].getXPos();
		double Y = BunchOfRayTips[i].getYPos();
		if (X<0 || Y<0 || X>ScreenWidth - 1 || Y>ScreenHeight - 1)
		{
			//RayTip越界则直接到下一条
			continue;
		}
		int LessX = int(X);//向下取X，作为判断基准点X坐标
		int LessY = int(Y);//向下取Y，作为判断基准点Y坐标
		int ActX = int(getSourcePosition().getXPos());
		int ActY = int(getSourcePosition().getYPos());//初始化为Source坐标如果达到激活条件的话，就用于存储对应坐标
		for (int x = LessX;x < LessX + 2;++x) 
		{
			for(int y=LessY;y<LessY+2;++y)
			{
				//位于激活半径内且不得越界
				if (((X - x) * (X - x) + (Y - y) * (Y - y) < JudgeRadius * JudgeRadius)&&((x>=0)&&(y>=0)&&(x<=ScreenWidth-1)&&(y<=ScreenHeight-1)))
				{
					ActX = x;
					ActY = y;
					Judger = true;
					break;
				}
			}
		}

		//如果没达到激活条件，就不再执行下面步骤
		if (Judger == false)continue;
		
		//判断Medium是否被激活过，如果没有，就将其激活
		int MediumPixelIndex = ScreenWidth * ActY + ActX;
		if (!IfActivated[MediumPixelIndex])
		{
			MediumLayer[MediumPixelIndex].GetActivated(timeOfNow,this->getSourceAmplitude(),this->getSourceFrequency(),this->getSourceInitialPhase());
			IfActivated[MediumPixelIndex] = true;
		}
	}
}