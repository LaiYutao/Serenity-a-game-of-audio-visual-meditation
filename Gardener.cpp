#include"Gardener.h"
#include"windows.h"

Gardener::Gardener():PlantingPoint(Point(ScreenWidth/2 -1,ScreenHeight/2 - 6 )) //��ʼ�����м�ƫ�ϵ�λ��
{
	//һ��ʼ��ÿ���㶼û�б����¹���,������������
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

std::vector<Field*>& Gardener::getRefCompoundField()
{
	return CompoundField;
}

void Gardener::UpdateCompoundHeight()
{
	//�Ƚ�������
	CompoundHeight.clear();
	CompoundHeight.resize(ScreenWidth * ScreenHeight, 0);

	for (int i = 0;i < ScreenWidth * ScreenHeight;++i)
	{
		for (std::vector<Medium>* mediumLayerPtr : CompoundMedium)
		{
			//�Ѹ�Medium���Height��������
			CompoundHeight[i] += (*mediumLayerPtr)[i].getHeight();
		}
	}
}

void Gardener::SelectPosition()
{
	double MovingSpeed = 5;//�ƶ��ٶ�Ϊ5 pixel per sec
	
	//����: ���¡�W�����Ҳ����磬�Ҵ�ʱû�а��š�J���͡�K����
	if ((GetAsyncKeyState((unsigned short)'W') & 0x8000)&&(PlantingPoint.getYPos()>0)&& !(GetAsyncKeyState((unsigned short)'J') & 0x8000)&& !(GetAsyncKeyState((unsigned short)'K') & 0x8000))
	{
		PlantingPoint.setYPos(PlantingPoint.getYPos()- MovingSpeed * FrameTime/1000);
	}

	//����: ���¡�S�����Ҳ����磬�Ҵ�ʱû�а��š�J���͡�K����
	if ((GetAsyncKeyState((unsigned short)'S') & 0x8000) && (PlantingPoint.getYPos() < ScreenHeight-3) && !(GetAsyncKeyState((unsigned short)'J') & 0x8000) && !(GetAsyncKeyState((unsigned short)'K') & 0x8000))
	{
		PlantingPoint.setYPos(PlantingPoint.getYPos() + MovingSpeed * FrameTime / 1000);
	}

	//���󣺰��¡�A�����Ҳ����磬�Ҵ�ʱû�а��š�J���͡�K����
	if ((GetAsyncKeyState((unsigned short)'A') & 0x8000) && (PlantingPoint.getXPos() > 0) && !(GetAsyncKeyState((unsigned short)'J') & 0x8000) && !(GetAsyncKeyState((unsigned short)'K') & 0x8000))
	{
		PlantingPoint.setXPos(PlantingPoint.getXPos() - MovingSpeed * FrameTime / 1000);
	}

	//���ң����¡�D�����Ҳ����磬�Ҵ�ʱû�а��š�J���͡�K����
	if ((GetAsyncKeyState((unsigned short)'D') & 0x8000) && (PlantingPoint.getXPos() < ScreenWidth-1) && !(GetAsyncKeyState((unsigned short)'J') & 0x8000) && !(GetAsyncKeyState((unsigned short)'K') & 0x8000))
	{
		PlantingPoint.setXPos(PlantingPoint.getXPos() + MovingSpeed * FrameTime / 1000);
	}
}

void Gardener::PlantIt(const double& timeOfNow)
{
	//ֱ�Ӵ���ʱ��Ĭ�ϲ���
	static double DefaultAmplitude = 1;
	static double DefaultFrequency = 0.25;//��Ӧ��Ƶ��Ϊ��/2����Ӧ����ΪT=4s
	static const double DefaultInitialPhase = PI / 2;//Source���������������
	static const double DefaultSpeed = 10;//�ó���λ�Ͳ��ٲ��䣻�������ӣ�
	
	static bool KeyJPressed = false;
	static bool KeyKPressed = false;

	//��λ����ȡ�������ĵ���Ϊ����λ�ã�PlantingPoint��SelectPosition���������Ѿ�����Ϊ����Խ����ˣ�
	double PickRadius = 0.5;
	double X = PlantingPoint.getXPos();
	double Y = PlantingPoint.getYPos();
	int LessX = int(X);//����ȡX����Ϊ�жϻ�׼��X����
	int LessY = int(Y);//����ȡY����Ϊ�жϻ�׼��Y����
	int PlantX = int(X);
	int PlantY = int(Y);//�ȱȽ�����س�ʼ��
	for (int x = LessX;x < LessX + 2;++x)
	{
		for (int y = LessY;y < LessY + 2;++y)
		{
			//λ�ڲ�׽�뾶��
			if ((X - x) * (X - x) + (Y - y) * (Y - y) < PickRadius * PickRadius)
			{
				PlantX = x;
				PlantY = y;
				break;
			}
		}
	}

	//����˴�û�����¹�Source
	if (SourceChecking[PlantY* ScreenWidth + PlantX]==0)
	{
		if ((GetAsyncKeyState((unsigned short)'J') & 0x8000)&& !KeyJPressed && !(GetAsyncKeyState((unsigned short)'K') & 0x8000))
		{
			//����J��������CircularField
			CompoundField.push_back(new CircularField(timeOfNow, PlantingPoint, DefaultAmplitude, DefaultFrequency, DefaultInitialPhase, DefaultSpeed));
			CompoundMedium.push_back(CompoundField[CompoundField.size()-1]->getPtrMediumLayer());
			SourceChecking[PlantY * ScreenWidth + PlantX] = CompoundField.size();//��¼��ʱ�����ĳ�����ţ��ӡ�1����ʼ
			NumOfCircularField++;
			KeyJPressed = true;//��ֹ�մ����Ϳ�ʼ���ڲ�������Ӧ���ȷſ�һ�£��ٿ����Ƿ�ʼ����
		}
		else if ((GetAsyncKeyState((unsigned short)'K') & 0x8000)&& !KeyKPressed && !(GetAsyncKeyState((unsigned short)'J') & 0x8000))
		{
			//����K��������SpiralField
			CompoundField.push_back(new SpiralField(timeOfNow, PlantingPoint, DefaultAmplitude, DefaultFrequency, DefaultInitialPhase, DefaultSpeed));
			CompoundMedium.push_back(CompoundField[CompoundField.size() - 1]->getPtrMediumLayer());
			SourceChecking[PlantY * ScreenWidth + PlantX] = CompoundField.size();//��¼��ʱ�����ĳ�����ţ��ӡ�1����ʼ
			NumOfSpiralField++;
			KeyKPressed = true;
		}
		else return;
	}
	else //����˴��Ѿ����¹�Source
	{
		if ((GetAsyncKeyState((unsigned short)'J') & 0x8000) && !(GetAsyncKeyState((unsigned short)'K') & 0x8000))
		{
			if (!KeyJPressed)
			{
				AdjustAmplitude(PlantX,PlantY);//������J�������ͽ����Amplitude�ĵ��ڣ��ɿ��ͽ�������
			}
		}
		else if ((GetAsyncKeyState((unsigned short)'K') & 0x8000)&& !(GetAsyncKeyState((unsigned short)'J') & 0x8000))
		{
			if(!KeyKPressed)
			{
				AdjustFrequency(PlantX,PlantY);//������K�������ͽ����Amplitude�ĵ��ڣ��ɿ��ͽ������ڣ�
			}
		}
		else
		{
			KeyJPressed = false;
			KeyKPressed = false; //������桰��ֹ�մ����͵���������
		}
	}
}

void Gardener::AdjustAmplitude(int plantX, int plantY)
{
	static bool KeyWPressed = false;
	static bool KeySPressed = false;
	static double AmplitudeUpperLimit = (GrayScale - 1) / 2;//����̫�ߣ�����Ч������
	
	//��ӦCompoundField�еı�ţ���0��ʼ��
	int NumField = SourceChecking[plantY * ScreenWidth + plantX]-1;
	
	//���¡�W������������ߣ������ܳ���AmplitudeUpperLimit
	if ((GetAsyncKeyState((unsigned short)'W') & 0x8000)&&(CompoundField[NumField]->getSourceAmplitude()< AmplitudeUpperLimit))
	{
		if (!KeyWPressed) //��ֹ��������
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

	//���¡�S������������ͣ������ܵ���0
	if ((GetAsyncKeyState((unsigned short)'S') & 0x8000) && (CompoundField[NumField]->getSourceAmplitude() >0))
	{
		if (!KeySPressed) //��ֹ��������
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
	static double FrequencyUpperLimit = 1;//̫��Ļ�����ʾ�����
	static double FrequencyLowerLimit = -1;//������Ļ�����������
	
	//��ӦCompoundField�еı�ţ���0��ʼ��
	int NumField = SourceChecking[plantY * ScreenWidth + plantX] - 1;

	//���¡�W������Ƶ�ʵ��ߣ������ܳ���FrequencyUpperLimit
	if ((GetAsyncKeyState((unsigned short)'W') & 0x8000) && (CompoundField[NumField]->getSourceFrequency() < FrequencyUpperLimit))
	{
		if (!KeyWPressed) //��ֹ��������
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

	//���¡�S������Ƶ�ʵ��ͣ������ܵ���0
	if ((GetAsyncKeyState((unsigned short)'S') & 0x8000) && (CompoundField[NumField]->getSourceFrequency() > FrequencyLowerLimit))
	{
		if (!KeySPressed) //��ֹ��������
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