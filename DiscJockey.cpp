#include "DiscJockey.h"
#include <algorithm>

DiscJockey::DiscJockey()
{
	MusicTypeFlag = true; //一开始默认十二音音簇模式；
	IfMute = false;
	int Offset = (GrayScale - 1) / 2; //灰度阶字符串索引所需的偏移量
	
	//初始化：所有的Medium的Height均为0
	HeightDistribution.resize(GrayScale, 0);

	HeightDistribution[0 + Offset] = ScreenWidth * ScreenHeight;

	CalculatedHertz = 0;

	// 配置波形格式
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;                  // 单声道
	wfx.nSamplesPerSec = 44100;         // 采样率
	wfx.wBitsPerSample = 16;            // 位深度
	wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.cbSize = 0;

}

void DiscJockey::CalculateHeightDistribution(const std::vector<double>& compoundHeight)
{
	int Offset = (GrayScale - 1) / 2; //灰度阶字符串索引所需的偏移量
	//计算分布前，先全部置为0
	for (int i = 0;i < GrayScale;++i)
	{
		HeightDistribution[i] = 0;
	}
	
	for (int i = 0;i < ScreenWidth * ScreenHeight;++i)
	{
		//采取和Monitor一样的整数化方式以保持统一
		int ShownHeight = (compoundHeight[i] >= 0) ? int(compoundHeight[i] + 0.5) : int(compoundHeight[i] - 0.5);
		
		//规范化，防止越界
		if (ShownHeight > (GrayScale - 1) / 2) ShownHeight = (GrayScale - 1) / 2;
		if ((-ShownHeight) > (GrayScale - 1) / 2) ShownHeight = -(GrayScale - 1) / 2;
		
		//计数
		HeightDistribution[ShownHeight + Offset]++;
	}
}

void DiscJockey::CalculateHertz()
{
	CalculatedHertz = 0;

	//微分白噪音模式：直接算加权平均的频率；
	for (int i = 0;i < 13;++i)
	{
		CalculatedHertz += HeightDistribution[i] * TwelveToneSeries[i];
	}

	CalculatedHertz /= ScreenWidth * ScreenHeight;
}

double DiscJockey::getCalculatedHertz() const
{
	return CalculatedHertz;
}

bool DiscJockey::getMusicType() const
{
	return MusicTypeFlag;
}

bool DiscJockey::getIfMute() const
{
	return IfMute;
}

void DiscJockey::DetectMusicTypeChange()
{
	static bool KeyJPressed = false;
	static bool KeyKPressed = false;
	if ((GetAsyncKeyState((unsigned short)'J') & 0x8000) && (GetAsyncKeyState((unsigned short)'K') & 0x8000))
	{
		if (!KeyJPressed && !KeyKPressed) //仍然是：防止按紧变来变去的情况
		{
			MusicTypeFlag = !MusicTypeFlag;
			KeyJPressed = true;
			KeyKPressed = true;
		}
	}
	else
	{
		KeyJPressed = false;
		KeyKPressed = false;
	}
}

void DiscJockey::DetectIfMute()
{
	static bool KeyMPressed = false;
	if (GetAsyncKeyState((unsigned short)'M') & 0x8000)
	{
		if (!KeyMPressed)
		{
			IfMute = !IfMute;
			KeyMPressed = true;
		}
	}
	else
	{
		KeyMPressed = false;
	}
}

//下面两个发声的函数需要用到
void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
	if (uMsg == WOM_DONE) {
	}
}

void DiscJockey::MakeWhiteNoise(const double& kDuration)
{
	result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, (DWORD_PTR)waveOutProc, 0, CALLBACK_FUNCTION);

	// 创建并填充音频数据
	const int numSamples = int(44100 * kDuration / 1000); //除一千用于转换单位
	wfx.nSamplesPerSec = int(numSamples / (double(kDuration) / 1000)) + 1;//抵消一些取整带来的误差（误差极小，0.001毫秒左右）
	short* audioData = new short[numSamples];
	for (int i = 0; i < numSamples; ++i)
	{
		double t = (double)i / wfx.nSamplesPerSec; // 取细分的时间
		audioData[i] = static_cast < short>(10000*sin(CalculatedHertz * 2 * PI * t));
	}

	//刨除末端部分振幅，试图缓解振幅绝对值比较高的时候突然停止，频率骤减，发出像煤气灶的声音；可以理解为某种程度上的低通滤波器
	if (kDuration > 10) //防止越界
	{
		double absminIntensity = 10000;
		int minNum = numSamples - int(0.01 * wfx.nSamplesPerSec);
		for (int i = numSamples - 1;i > numSamples - int(0.01 * wfx.nSamplesPerSec);--i)
		{
			if (abs(audioData[i]) < absminIntensity)
			{
				absminIntensity = abs(audioData[i]);
				minNum = i;
			}
			if (absminIntensity <= 100) { //减少遍历次数
				break;
			}
		}
		for (int i = minNum + 1;i < numSamples; ++i)
		{
			audioData[i] = static_cast<short>(absminIntensity * (numSamples - i) / (numSamples - minNum - 1));
		}
	}

	header.lpData = (LPSTR)audioData;
	header.dwBufferLength = numSamples * sizeof(short);
	header.dwFlags = 0;
	header.dwLoops = 0;

	//写入并播放
	result = waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	result = waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	
}

void DiscJockey::MakeClusters(const double& kDuration)
{
	result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, (DWORD_PTR)waveOutProc, 0, CALLBACK_FUNCTION);
	
	// 创建并填充音频数据
	const int numSamples = int(44100 * kDuration/1000); //除一千用于转换单位
	wfx.nSamplesPerSec = int(numSamples / (double(kDuration) / 1000))+1;//抵消一些取整带来的误差（误差极小，0.001毫秒左右）
	short* audioData = new short[numSamples];
	for (int i = 0; i < numSamples; ++i) 
	{
		double t = (double)i / wfx.nSamplesPerSec; // 取细分的时间
		double Intensity = 0;
		double NumOfNote = 0;
		//选取十二音并叠上去；由于用的是正弦函数，Height==0对应的0赫兹，对应的振幅就直接是零，很方便
		int min = 5;//输出高度分布前五大的音，降低音的复杂度；也减少计算量
		//创建一个副本用来找第min大的元素
		std::vector<int> SortHD = HeightDistribution;
		// 找到第 min 大的元素，并将其放置到正确位置
		std::nth_element(SortHD.begin(), SortHD.begin() + min-1, SortHD.end(), std::greater<int>());
		for (int j = 0;j < 13;j++)
		{
			//如果对应高度没有点，就直接跳过
			if (HeightDistribution[j] == 0)continue;
			//大于第min大对应的数值，就加入Intensity中；
			if ((HeightDistribution[j] >= SortHD[min - 1])&&(NumOfNote<min))//防止并列第min的情况（这种情况应该不多，但如果出现的话，则选取更低的音）
			{
				//第一项是基音，后面加了四项泛音（由网上搜索的钢琴频谱图换算而来），试图粗略地模拟钢琴音色
				Intensity += HeightDistribution[j] * sin(TwelveToneSeries[j] * 2 * PI * t) +
					0.1 * HeightDistribution[j] * sin(TwelveToneSeries[j] * 2 * 2 * PI * t) + 0.056 * HeightDistribution[j] * sin(TwelveToneSeries[j] * 3 * 2 * PI * t) +
					0.042 * HeightDistribution[j] * sin(TwelveToneSeries[j] * 4 * 2 * PI * t) + 0.037 * HeightDistribution[j] * sin(TwelveToneSeries[j] * 5 * 2 * PI * t);
				NumOfNote++;
			}
		}
		
		Intensity /= ScreenWidth * ScreenHeight; //振幅取均值
		Intensity *= 12000;// 适当放大，使得播放正常,音量和白噪音模式接近即可
		audioData[i] = static_cast<short>(Intensity); 
	}

	//刨除末端部分振幅，试图缓解振幅比较高的时候突然停止，频率骤减，发出像煤气灶的声音；可以理解为某种程度上的低通滤波器（但效果没想象中好）
	if(kDuration>10) //防止越界
	{
		double absminIntensity = 10000;
		int minNum = numSamples - int(0.01 * wfx.nSamplesPerSec);
		for (int i = numSamples - 1;i > numSamples - int(0.01 * wfx.nSamplesPerSec);--i)
		{
			if (abs(audioData[i]) < absminIntensity)
			{
				absminIntensity = abs(audioData[i]);
				minNum = i;
			}
			if (absminIntensity <=100) {//减少遍历次数
				break;
			}
		}
		for (int i = minNum + 1;i < numSamples; ++i)
		{
			audioData[i] = static_cast<short>(absminIntensity * (numSamples - i) / (numSamples - minNum - 1));
		}
	}
	
	header.lpData = (LPSTR)audioData;
	header.dwBufferLength = numSamples * sizeof(short);
	header.dwFlags = 0;
	header.dwLoops = 0;

	//写入并播放
	result = waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	result = waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	
}