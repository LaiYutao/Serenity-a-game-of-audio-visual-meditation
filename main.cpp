#include"Gardener.h"
#include"Monitor.h"
#include"DiscJockey.h"
#include<fstream>
#include<string>
#include<chrono>
#include<random>

//包含主loop在内的主干，控制游戏主界面
void Act(ScreenManager TheScreenManager)
{
	//设定时间原点
	double TimeOrigin = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	auto tp1 = std::chrono::high_resolution_clock::now();
	auto tp2 = std::chrono::high_resolution_clock::now();

	//初始化三个单例
	Gardener TheGardener;
	Monitor TheMonitor;
	DiscJockey TheDiscJockey;

	//主循环
	while (1) 
	{
		//获取ElapsedTime(单位为毫秒)
		tp2 = std::chrono::high_resolution_clock::now();
		auto ElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(tp2 - tp1).count();
		tp1 = tp2;

		//获取时间轴中当前时间(单位为秒)
		double TimeOfNow = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() - TimeOrigin;
		
		//TheDiscJockey检测是否处于静音模式
		TheDiscJockey.DetectIfMute();

		//TheDiscJockey检测MusicType是否改变
		TheDiscJockey.DetectMusicTypeChange();

		//如果不是处于静音模式，则创建对应音乐模式的线程
		std::thread MusicMode;
		if (TheDiscJockey.getIfMute() == false)
		{
			if (TheDiscJockey.getMusicType() == true)
			{
				//十二音音簇模式
				MusicMode = std::thread([&TheDiscJockey, ElapsedTime]() { TheDiscJockey.MakeClusters(FrameTime*0.4); }); 
			}
			else
			{
				//微分白噪音模式
				MusicMode = std::thread([&TheDiscJockey, ElapsedTime]() { TheDiscJockey.MakeWhiteNoise(FrameTime*0.4); });
			}
		}

		//启动PlantingPoint的选择
		TheGardener.SelectPosition();

		//启动“种植”功能
		TheGardener.PlantIt(TimeOfNow);

		//遍历CompoundField中的每一个场,让每个场开始在对应Medium层中对四周的Medium进行激活
		for (auto TheField : TheGardener.getRefCompoundField())
		{
			TheField->ActivateMedium(TimeOfNow, double(FrameTime)/1000);
		}
		
		//让每个场对应的Medium层的每个Medium都振动（改变Height）
		for (auto TheField : TheGardener.getRefCompoundField())
		{
			for (int i = 0;i < ScreenWidth * ScreenHeight;++i) 
			{
				(*TheField->getPtrMediumLayer())[i].Vibrate(TimeOfNow);
			}
		}

		//TheGardener更新CompoundHeight
		TheGardener.UpdateCompoundHeight();

		//TheMonitor接受更新后的CompoundHeight
		TheMonitor.SetCompoundHeight(TheGardener.getCompoundHeight());

		//TheMonitor将Height转化为字符
		TheMonitor.ChangeIntoPixel(TheScreenManager.getRefScreenBuffer());

		//TheMonitor加上PlantingPoint的显示
		TheMonitor.AddPlantingPoint(TheScreenManager.getRefScreenBuffer(), TheGardener.getPlantingPoint());

		//TheMonitor加上状态栏的显示
		TheMonitor.AddStatusBar(TheScreenManager.getScreenBuffer(),TheScreenManager.getRefScreenShow(),TheGardener.getPlantingPoint(),TheGardener.getNumOfCircularField(),TheGardener.getNumOfSpiralField());

		//TheDiscJockey接收CompoundHeight
		TheDiscJockey.CalculateHeightDistribution(TheGardener.getCompoundHeight());
		
		//TheDiscJockey计算在微分白噪音模式下发出赫兹
		TheDiscJockey.CalculateHertz();

		//等待MusicMode进程完成
		if (MusicMode.joinable())
		{
			MusicMode.join();
		}
		
		//TheScreenManager输出图像
		TheScreenManager.ShowActImage();

		// 控制帧率；
		if (ElapsedTime < FrameTime)//单位为毫秒
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(FrameTime - ElapsedTime));
		}
	}
}

//进入封面页前的操作提示指引
void Tips()
{
	using namespace std;
	cout << "基础设置：（win11操作系统）" << endl;
	cout << "1.点击窗口上方标题栏中类似“V”的符号（偏左，在“+”右边），再点击设置。" << endl;
	cout << "2.在“启动”选项中，在右侧第二行“默认终端应用程序”中，点击最右端类似“V”的按键，将选项设置为“Windows控制台主机”。右下角处点击蓝色键“保存”。" << endl;
	cout << "3.设置完成。请关闭并重新启动此程序。" << endl;
	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "简介：    【!*!请先向上滑动鼠标滚轮，到页面最上方，查看“基础设置”并操作!*!】" << endl;
	cout<< "操纵PlantingPoint（一开始位于中央）进行移动，选择位置，种下场源（两种可选），并按需调节振幅或者频率。反复操作，创造出独一无二、不可复制的视听景观！" << endl;
	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "操作指南：【!*!请先向上滑动鼠标滚轮，到页面最上方，查看“基础设置”并操作!*!】" << endl;
	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "    W/S/A/D\t|控制PlantingPoint 向上/向下/向左/向右" << endl;
	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "       J\t|种下CircularField（圆形波场）" << endl;
	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "       K\t|种下SpiralField（螺旋形场）" << endl;
	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "  长按J+单点W\t|升高此位置的场对应振幅" << endl;
	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "  长按J+单点S\t|降低此位置的场对应振幅" << endl;
	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "  长按K+单点W\t|升高此位置的场对应频率" << endl;
	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "  长按K+单点S\t|降低此位置的场对应频率（频率为0则静止，为负则反向运动）" << endl;
	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "      J+K\t|切换音乐模式（两种可选）" << endl;
	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "       M\t|静音/取消静音" << endl;
	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "     提示1\t|视设备性能不同，如果画面卡顿，可以按M键静音以缓解" << endl;
	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "     提示2\t|不要连续快速种下场源，按下按键后保持一下，并隔几秒再种另一个" << endl;
	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "     提示3\t|进入游戏主画面后（封面及过渡页之后），下方有一栏数据栏，可供查看" << endl;
	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "在接下来的游戏过程中，请不要调节窗口大小或者全屏，如不小心操作了，请关闭并重启程序" << endl;
	cout << "Have a good time!" << endl;
	system("pause");
}

//仅用于CoverPage()，用于覆盖动态的字母
void AddCharacter(ScreenManager& TheScreenManager,std::vector<char> Character,int X, int Y) //由于本函数仅在下面CoverPage函数单一位置反复调用；故其实可以视为该函数本身的一部分，因此形参都用了大写开头
{
	int x = X;
	int y = Y;

	for (int i = 0;i < Character.size();++i)
	{
		if (Character[i] == '\n')
		{
			y++;
			x = X;
		}
		else
		{
			TheScreenManager.getRefScreenBuffer()[y * ScreenWidth + x] = Character[i];
			x++;
		}
	}
}

//仅用于CoverPage()，用于读取字母字符画文件
void ReadFile(std::vector<char>& Character, std::string Directory)
{
	char c;
	std::ifstream inFile(Directory);
	while (inFile.get(c))
	{
		Character.push_back(c);
	}
	inFile.close();
}

//动态封面页
void CoverPage(ScreenManager TheScreenManager)
{
	//读取背景文件
	std::ifstream inFile("txtfile\\CoverPage.txt");
	std::vector<char> BackGround;
	char c;
	while (inFile.get(c))
	{
		if (c != '\n') //消除换行符
		{
			//将文件内容写入静态BackGround备用
			BackGround.push_back(c);
		}
	}
	inFile.close();

	//读取字母文件：声明
	std::vector<char> Sl; //向左倒的S
	std::vector<char> Sr; //向右倒的S
	std::vector<char> el; //向左倒的e
	std::vector<char> er; //向右倒的e
	std::vector<char> rl; //向左倒的r
	std::vector<char> rr; //向右倒的r
	std::vector<char> nl; //向左倒的n
	std::vector<char> nr; //向右倒的n
	std::vector<char> il; //向左倒的i
	std::vector<char> ir; //向右倒的i
	std::vector<char> tl; //向左倒的t
	std::vector<char> tr; //向右倒的t
	std::vector<char> yl; //向左倒的y
	std::vector<char> yr; //向右倒的y
	std::vector<char> tips; //提示词

	//读取字母文件：读取
	{
		ReadFile(Sl, "txtfile\\character\\S l.txt");
		ReadFile(Sr, "txtfile\\character\\S r.txt");
		ReadFile(el, "txtfile\\character\\e l.txt");
		ReadFile(er, "txtfile\\character\\e r.txt");
		ReadFile(rl, "txtfile\\character\\r l.txt");
		ReadFile(rr, "txtfile\\character\\r r.txt");
		ReadFile(nl, "txtfile\\character\\n l.txt");
		ReadFile(nr, "txtfile\\character\\n r.txt");
		ReadFile(il, "txtfile\\character\\i l.txt");
		ReadFile(ir, "txtfile\\character\\i r.txt");
		ReadFile(tl, "txtfile\\character\\t l.txt");
		ReadFile(tr, "txtfile\\character\\t r.txt");
		ReadFile(yl, "txtfile\\character\\y l.txt");
		ReadFile(yr, "txtfile\\character\\y r.txt");
		ReadFile(tips, "txtfile\\Tips.txt");
	}
	
	bool Out = false;
	bool KeySpacePressed = true;//防止上来就按着SPACE，直接把封面跳过了

	//设定时间原点
	double TimeOrigin = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	double TimeOfNow = 0;

	//写入静态BackGround
	TheScreenManager.getRefScreenBuffer() = BackGround;

	//加入随机显示的星点，丰富画面
	std::srand(std::time(0));
	for (int i = 0;i < 1000;++i)
	{
		int X = std::rand() % TheScreenManager.getScreenWidth();
		int Y = std::rand() % TheScreenManager.getScreenHeight();

		if (!(((X >= 73) && (X <= 158) && (Y >= 45) && (Y <= 49)) || ((X >= 60) && (X <= 172) && (Y >= 55) && (Y <= 62)))) //避免遮挡文字
		{
			TheScreenManager.getRefScreenBuffer()[Y * TheScreenManager.getScreenWidth() + X] = 'o';
		}
	}

	//"PRESS SPACE TO CONTINUE"是否显示；因为要做闪烁的提示词
	bool IfShowPressSpace = true;

	//输出Page
	while (Out == false)
	{
		//获取时间轴中当前时间(单位为秒)
		TimeOfNow = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() - TimeOrigin;
		
		//检查是否按下空格键
		if (GetAsyncKeyState((unsigned short)' ') & 0x8000)
		{
			if (!KeySpacePressed)
			{
				Out = true;
				KeySpacePressed = true;
			}
		}
		else
		{
			KeySpacePressed = false;
		}
		
		//写入字母
		if(2*TimeOfNow-int(2*TimeOfNow)<=0.002) //理论上离每0.5秒足够近，就变换一次；这样一来，也保证了空格检测的灵敏度；
		{                                       //（实际上，由于没有控制帧率，会有所偏差，但对于封面页没什么所谓，还增加了随机性）
			//从尾到头，从y开始；（由于y尾部横向延伸比较大）
			if (rand() % 2)AddCharacter(TheScreenManager, yl, 183, 29); //y所应该出现的区域的左上角
			else AddCharacter(TheScreenManager, yr, 183, 29);

			if (rand() % 2)AddCharacter(TheScreenManager, tl, 167, 27); //t所应该出现的区域的左上角
			else AddCharacter(TheScreenManager, tr, 167, 27);

			if (rand() % 2)AddCharacter(TheScreenManager, il, 153, 25); //i所应该出现的区域的左上角
			else AddCharacter(TheScreenManager, ir, 153, 25);

			if (rand() % 2)AddCharacter(TheScreenManager, nl, 125, 29); //n所应该出现的区域的左上角
			else AddCharacter(TheScreenManager, nr, 125, 29);

			if (rand() % 2)AddCharacter(TheScreenManager, el, 102, 29); //e所应该出现的区域的左上角
			else AddCharacter(TheScreenManager, er, 102, 29);

			if (rand() % 2)AddCharacter(TheScreenManager, rl, 81, 29); //r所应该出现的区域的左上角
			else AddCharacter(TheScreenManager, rr, 81, 29);
			
			if (rand() % 2)AddCharacter(TheScreenManager, el, 58, 29); //e所应该出现的区域的左上角
			else AddCharacter(TheScreenManager, er, 58, 29);

			if (rand() % 2)AddCharacter(TheScreenManager, Sl, 30, 22); //S所应该出现的区域的左上角
			else AddCharacter(TheScreenManager, Sr, 30, 22);
		}
		
		//写入PRESS SPACE提示词
		if (4 * TimeOfNow - int(4 * TimeOfNow) <= 0.004) //理论每0.25秒闪一次（实际上，由于没有控制帧率，会有所偏差，但对于封面页没什么所谓，还增加了随机性）
		{
			if (IfShowPressSpace == true) //如果有，就用空格覆盖
			{
				for (int i = 65 * TheScreenManager.getScreenWidth();i < 69 * TheScreenManager.getScreenWidth() + TheScreenManager.getScreenWidth() - 1;++i)//提示词区域从（0,65）到（ScreenWidth-1,69）
				{
					TheScreenManager.getRefScreenBuffer()[i] = ' ';
				}
				IfShowPressSpace = false;
			}
			else //如果没有，就打上提示词
			{
				AddCharacter(TheScreenManager, tips, 0, 65);
				IfShowPressSpace = true;
			}
		}

		//输出页面
		TheScreenManager.ShowPageImage();
	}
}

//开始前的过渡页
void BridgePage(ScreenManager TheScreenManager)
{
	//设定时间原点
	double TimeOrigin = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	double TimeOfNow = 0;
	//读取文件
	std::ifstream inFile("txtfile\\BridgePage.txt");
	char c;
	while (inFile.get(c))
	{
		TheScreenManager.getRefScreenBuffer().push_back(c);
	}
	inFile.close();

	//设定此页持续时间
	std::uniform_real_distribution<double> dis(0.5, 1.5);//比较随意的时间区间，从中随机选择，作为此页显示时间
	std::default_random_engine gen;
	double Duration= dis(gen);

	//输出page
	while (TimeOfNow < Duration)
	{
		//获取时间轴中当前时间(单位为秒)
		TimeOfNow = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() - TimeOrigin;
		//输出页面
		TheScreenManager.ShowPageImage();
	}
}

int main() 
{
	Tips();
	ScreenManager PageManager(ScreenWidth, ScreenHeight / 2, FontWidth, FontHeight * 2);//窗口的物理长宽和游戏主画面一致
	CoverPage(PageManager);
	BridgePage(PageManager);
	ScreenManager TheScreenManager;
	Act(TheScreenManager);
	return 0;
}