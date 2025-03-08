#include"Monitor.h"

Monitor::Monitor()
{
	CompoundHeight.resize(ScreenWidth * ScreenHeight, 0);
}

std::vector<double> Monitor::getCompoundHeight() const
{
	return CompoundHeight;
}

void Monitor::SetCompoundHeight(const std::vector<double>& compoundHeight)
{
	CompoundHeight = compoundHeight;
}

void Monitor::ChangeIntoPixel(std::vector<char>& buffer)
{
	int Offset = (GrayScale - 1) / 2; //灰度阶字符串索引所需的偏移量
	for (int i = 0;i < ScreenWidth * ScreenHeight;++i)
	{
		//粗略的四舍五入，避免对于一些波，振幅最高的情形取不到
		int ShownHeight = (CompoundHeight[i] >= 0) ? int(CompoundHeight[i] + 0.5) :int(CompoundHeight[i] - 0.5);
		if(ShownHeight + Offset>GrayScale-1)
		{
			buffer.push_back('@');
		}
		else if(ShownHeight + Offset < 0)
		{
			buffer.push_back(' ');
		}
		else
		{
			buffer.push_back(" .,:;*o%&#H$@"[ShownHeight + Offset]); //经过实验得出来的灰度阶序列，基本准确；
                                                                     //同时十三个灰度阶，抛出Height==0对应的情况，刚好能对应十二个音，很方便进行音乐的映射
		}
	}
}

void Monitor::AddPlantingPoint(std::vector<char>& buffer, const Point& plantingPoint)
{
	//定位器：取离得最近的点作为考虑位置（PlantingPoint在SelectPosition函数那里已经设置为不会越界的了）
	double PickRadius = 0.5;
	double X = plantingPoint.getXPos();
	double Y = plantingPoint.getYPos();
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
	
	//围合PlantingPoint
	buffer[(PlantY-1) * ScreenWidth + PlantX] = '-';
	buffer[(PlantY+1) * ScreenWidth + PlantX] = '-';
	buffer[PlantY * ScreenWidth + PlantX] = 'X';
	buffer[PlantY  * ScreenWidth + PlantX-1] = '|';
	buffer[PlantY  * ScreenWidth + PlantX+1] = '|';
}

void Monitor::AddStatusBar(std::vector<char> buffer,char*& screenShow,const Point& plantingPoint, const int& numOfCircularField, const int& numOfSpiralField)
{
	//加上一行分割线并将最底下两行清空
	for (int i = (ScreenHeight - 1) * ScreenWidth;i < ScreenHeight * ScreenWidth;++i)
	{
		if ((i - ScreenWidth) % 2 == 0) {
			buffer[i - ScreenWidth*2] = '=';
		}
		else
		{
			buffer[i - ScreenWidth*2] = '|';
		}
		buffer[i - ScreenWidth] = ' ';
		buffer[i] = ' ';
	}
	
	//转化数据类型，便于下一步的格式化：将vector<char>中元素一一赋值给char*
	for (int i = 0;i <  ScreenWidth * ScreenHeight;++i)
	{
		screenShow[i] = buffer[i];
	}

	sprintf_s(&screenShow[(ScreenHeight - 2) * ScreenWidth], ScreenWidth-1, "  PlantPoint: X=%3.2f, Y=%3.2f;   NumOfFields: CircularField=%2.i, SpiralField=%2.i, Total=%2.i    \0", plantingPoint.getXPos(), plantingPoint.getYPos(), numOfCircularField, numOfSpiralField, numOfCircularField + numOfSpiralField);
}
