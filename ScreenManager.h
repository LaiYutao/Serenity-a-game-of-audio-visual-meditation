#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H
#include <iostream>
#include <vector>
#include <windows.h>
#include <string>
#include <fstream>
#include <cmath>
#include <thread>
//以下宏常量都是针对游戏主画面的（不含封面页、过渡页）
#define ScreenWidth 240 //游戏主画面的宽
#define ScreenHeight 150 //游戏主画面的高
#define FontWidth 6 //游戏主画面的字宽
#define FontHeight 6 //游戏主画面的字高
#define FrameTime 100 //单位为毫秒
#define GrayScale 13//灰度阶阶数，直接影响细腻程度

class ScreenManager
{
public:
    //默认参数为游戏主画面的参数
    ScreenManager(int screenWidth= ScreenWidth,int screenHeight= ScreenHeight,int fontWidth= FontWidth,int fontHeight= FontHeight);
    void ChangeBufferForm();
    void ShowActImage();
    void ShowPageImage(); // 每次显示后不清空ScreenBuffer

    void setScreenShow(char* screen);
    void SetEmptyBuffer();
    std::vector<char>& getRefScreenBuffer();
    std::vector<char> getScreenBuffer();
    char*& getRefScreenShow();
    int getScreenWidth()const;
    int getScreenHeight()const;

private:
    HANDLE hConsole;
    SMALL_RECT rectWindow;
    char* ScreenShow;
    std::vector<char> ScreenBuffer;
    //用于游戏主画面之外画面的控制台宽高、字体宽高设置用私有成员，加m以示区分；
    int m_ScreenWidth;
    int m_ScreenHeight;
    int m_FontWidth;
    int m_FontHeight;
};
#endif // SCREENMANAGER_H
