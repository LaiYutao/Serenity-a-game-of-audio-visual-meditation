#include"Gardener.h"
#include"windows.h"

Gardener::Gardener():PlantingPoint(Point(ScreenWidth/2 -1,ScreenHeight/2 - 6 )) //初始点在中间偏上的位置
{
	//一开始，每个点都没有被种下过场,各变量都置零
	for (int i = 0;i < ScreenWidth * ScreenHeight;++i)
	{
		SourceChecking[i] =0;
	}
	CompoundHeight.resize(ScreenWidth * ScreenHeight, 0);
	NumOfCircularField = 0;
	NumOfSpiralField = 0;
}

Point Gardener::getPlantingPoint() const
{
	return PlantingPoint;
}

int Gardener::getNumOfCircularField() const
{
	return NumOfCircularField;
}

int Gardener::getNumOfSpiralField() const
{
	return NumOfSpiralField;
}

std::vector<double> Gardener::getCompoundHeight() const
{
	return CompoundHeight;
}

std::vector<std::unique_ptr<Field>>& Gardener::getRefCompoundField()
{
	return CompoundField;
}

void Gardener::UpdateCompoundHeight()
{
	//先进行重置
	CompoundHeight.clear();
	CompoundHeight.resize(ScreenWidth * ScreenHeight, 0);

	for (int i = 0;i < ScreenWidth * ScreenHeight;++i)
	{
		for (std::vector<Medium>* mediumLayerPtr : CompoundMedium)
		{
			//把各Medium层的Height叠加起来
			CompoundHeight[i] += (*mediumLayerPtr)[i].getHeight();
		}
	}
}

void Gardener::SelectPosition()
{
	double MovingSpeed = 5;//移动速度为5 pixel per sec
	
	//向上: 按下“W”，且不超界，且此时没有按着“J”和“K”；
	if ((GetAsyncKeyState((unsigned short)'W') & 0x8000)&&(PlantingPoint.getYPos()>0)&& !(GetAsyncKeyState((unsigned short)'J') & 0x8000)&& !(GetAsyncKeyState((unsigned short)'K') & 0x8000))
	{
		PlantingPoint.setYPos(PlantingPoint.getYPos()- MovingSpeed * FrameTime/1000);
	}

	//向下: 按下“S”，且不超界，且此时没有按着“J”和“K”；
	if ((GetAsyncKeyState((unsigned short)'S') & 0x8000) && (PlantingPoint.getYPos() < ScreenHeight-3) && !(GetAsyncKeyState((unsigned short)'J') & 0x8000) && !(GetAsyncKeyState((unsigned short)'K') & 0x8000))
	{
		PlantingPoint.setYPos(PlantingPoint.getYPos() + MovingSpeed * FrameTime / 1000);
	}

	//向左：按下“A”，且不超界，且此时没有按着“J”和“K”；
	if ((GetAsyncKeyState((unsigned short)'A') & 0x8000) && (PlantingPoint.getXPos() > 0) && !(GetAsyncKeyState((unsigned short)'J') & 0x8000) && !(GetAsyncKeyState((unsigned short)'K') & 0x8000))
	{
		PlantingPoint.setXPos(PlantingPoint.getXPos() - MovingSpeed * FrameTime / 1000);
	}

	//向右：按下“D”，且不超界，且此时没有按着“J”和“K”；
	if ((GetAsyncKeyState((unsigned short)'D') & 0x8000) && (PlantingPoint.getXPos() < ScreenWidth-1) && !(GetAsyncKeyState((unsigned short)'J') & 0x8000) && !(GetAsyncKeyState((unsigned short)'K') & 0x8000))
	{
		PlantingPoint.setXPos(PlantingPoint.getXPos() + MovingSpeed * FrameTime / 1000);
	}
}

void Gardener::PlantIt(const double& timeOfNow)
{
	//直接创建时的默认参数
	static double DefaultAmplitude = 1;
	static double DefaultFrequency = 0.25;//对应角频率为π/2；对应周期为T=4s
	static const double DefaultInitialPhase = PI / 2;//Source处振动起初是向下振动
	static const double DefaultSpeed = 10;//让初相位和波速不变；避免冗杂；
	
	static bool KeyJPressed = false;
	static bool KeyKPressed = false;

	//定位器：取离得最近的点作为考虑位置（PlantingPoint在SelectPosition函数那里已经设置为不会越界的了）
	double PickRadius = 0.5;
	double X = PlantingPoint.getXPos();
	double Y = PlantingPoint.getYPos();
	int LessX = int(X);//向下取X，作为判断基准点X坐标
	int LessY = int(Y);//向下取Y，作为判断基准点Y坐标
	int PlantX = int(X);
	int PlantY = int(Y);//先比较随意地初始化
	for (int x = LessX;x < LessX + 2;++x)
	{
		for (int y = LessY;y < LessY + 2;++y)
		{
			//位于捕捉半径内
			if ((X - x) * (X - x) + (Y - y) * (Y - y) < PickRadius * PickRadius)
			{
				PlantX = x;
				PlantY = y;
				break;
			}
		}
	}

	//如果此处没有种下过Source
	if (SourceChecking[PlantY* ScreenWidth + PlantX]==0)
	{
		if ((GetAsyncKeyState((unsigned short)'J') & 0x8000)&& !KeyJPressed && !(GetAsyncKeyState((unsigned short)'K') & 0x8000))
		{
			//按下J键，种下CircularField
			CompoundField.push_back(std::make_unique<CircularField>(timeOfNow, PlantingPoint, DefaultAmplitude, DefaultFrequency, DefaultInitialPhase, DefaultSpeed));
			CompoundMedium.push_back(CompoundField[CompoundField.size()-1]->getPtrMediumLayer());
			SourceChecking[PlantY * ScreenWidth + PlantX] = CompoundField.size();//记录此时创建的场的序号，从“1”开始
			NumOfCircularField++;
			KeyJPressed = true;//防止刚创建就开始调节参数；而应该先放开一下，再考虑是否开始调节
		}
		else if ((GetAsyncKeyState((unsigned short)'K') & 0x8000)&& !KeyKPressed && !(GetAsyncKeyState((unsigned short)'J') & 0x8000))
		{
			//按下K键，种下SpiralField
			CompoundField.push_back(std::make_unique<SpiralField>(timeOfNow, PlantingPoint, DefaultAmplitude, DefaultFrequency, DefaultInitialPhase, DefaultSpeed));
			CompoundMedium.push_back(CompoundField[CompoundField.size() - 1]->getPtrMediumLayer());
			SourceChecking[PlantY * ScreenWidth + PlantX] = CompoundField.size();//记录此时创建的场的序号，从“1”开始
			NumOfSpiralField++;
			KeyKPressed = true;
		}
		else return;
	}
	else //如果此处已经种下过Source
	{
		if ((GetAsyncKeyState((unsigned short)'J') & 0x8000) && !(GetAsyncKeyState((unsigned short)'K') & 0x8000))
		{
			if (!KeyJPressed)
			{
				AdjustAmplitude(PlantX,PlantY);//按紧“J”键，就进入对Amplitude的调节；松开就结束调节
			}
		}
		else if ((GetAsyncKeyState((unsigned short)'K') & 0x8000)&& !(GetAsyncKeyState((unsigned short)'J') & 0x8000))
		{
			if(!KeyKPressed)
			{
				AdjustFrequency(PlantX,PlantY);//按紧“K”键，就进入对Amplitude的调节；松开就结束调节；
			}
		}
		else
		{
			KeyJPressed = false;
			KeyKPressed = false; //配合上面“防止刚创建就调整参数”
		}
	}
}

void Gardener::AdjustAmplitude(int plantX, int plantY)
{
	static bool KeyWPressed = false;
	static bool KeySPressed = false;
	static double AmplitudeUpperLimit = (GrayScale - 1) / 2;//不能太高，否则效果不好
	
	//对应CompoundField中的编号，从0开始；
	int NumField = SourceChecking[plantY * ScreenWidth + plantX]-1;
	
	//按下“W”，把振幅调高；但不能超过AmplitudeUpperLimit
	if ((GetAsyncKeyState((unsigned short)'W') & 0x8000)&&(CompoundField[NumField]->getSourceAmplitude()< AmplitudeUpperLimit))
	{
		if (!KeyWPressed) //防止按键连续
		{
			CompoundField[NumField]->setSourceAmplitude(CompoundField[NumField]->getSourceAmplitude() + 1);
			CompoundField[NumField]->ClearIfActivated();
			KeyWPressed = true;
		}
	}
	else
	{
		KeyWPressed = false;
	}

	//按下“S”，把振幅调低；但不能低于0
	if ((GetAsyncKeyState((unsigned short)'S') & 0x8000) && (CompoundField[NumField]->getSourceAmplitude() >0))
	{
		if (!KeySPressed) //防止按键连续
		{
			CompoundField[NumField]->setSourceAmplitude(CompoundField[NumField]->getSourceAmplitude() - 1);
			CompoundField[NumField]->ClearIfActivated();
			KeySPressed = true;
		}
	}
	else
	{
		KeySPressed = false;
	}
}

void Gardener::AdjustFrequency(int plantX, int plantY)
{
	static bool KeyWPressed = false;
	static bool KeySPressed = false;
	static double FrequencyUpperLimit = 1;//太快的话，显示不清楚
	static double FrequencyLowerLimit = -1;//低于零的话，就是逆向
	
	//对应CompoundField中的编号，从0开始；
	int NumField = SourceChecking[plantY * ScreenWidth + plantX] - 1;

	//按下“W”，把频率调高；但不能超过FrequencyUpperLimit
	if ((GetAsyncKeyState((unsigned short)'W') & 0x8000) && (CompoundField[NumField]->getSourceFrequency() < FrequencyUpperLimit))
	{
		if (!KeyWPressed) //防止按键连续
		{
			CompoundField[NumField]->setSourceFrequency(CompoundField[NumField]->getSourceFrequency() + 0.05);
			CompoundField[NumField]->ClearIfActivated();
			KeyWPressed = true;
		}
	}
	else
	{
		KeyWPressed = false;
	}

	//按下“S”，把频率调低；但不能低于0
	if ((GetAsyncKeyState((unsigned short)'S') & 0x8000) && (CompoundField[NumField]->getSourceFrequency() > FrequencyLowerLimit))
	{
		if (!KeySPressed) //防止按键连续
		{
			CompoundField[NumField]->setSourceFrequency(CompoundField[NumField]->getSourceFrequency() - 0.05);
			CompoundField[NumField]->ClearIfActivated();
			KeySPressed = true;
		}
	}
	else
	{
		KeySPressed = false;
	}
}