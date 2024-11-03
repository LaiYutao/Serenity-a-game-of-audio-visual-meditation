#ifndef MONITOR_H
#define MONITOR_H //用于综合处理每个MediumLayer中每一个Medium的Height
#include"Medium.h"                 
#include"ScreenManager.h"
#include<vector>

class Monitor 
{
public:
    Monitor();
    std::vector<double> getCompoundHeight()const;
    void SetCompoundHeight(const std::vector<double>& compoundHeight);
    void ChangeIntoPixel(std::vector<char>& buffer);//接受每个Medium的Height,然后映射到对应字符。
    void AddPlantingPoint(std::vector<char>& buffer,const Point& plantingPoint);
    void AddStatusBar(std::vector<char> buffer,char*& screenShow,const Point& plantingPoint, const int& numOfCircularField, const int& numOfSpiralField);

private:
    std::vector<double> CompoundHeight;
};
#endif // MONITOR_H