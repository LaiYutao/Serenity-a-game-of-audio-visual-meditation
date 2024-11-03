#ifndef DISCJOCKEY_H
#define DISCJOCKEY_H 
#include<vector>
#include"ScreenManager.h"
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")
#define PI 3.14159
#define PI_ 0.318309


class DiscJockey
{
public:
	DiscJockey();
	void CalculateHeightDistribution(const std::vector<double>& compoundHeight);
	void CalculateHertz();
	double getCalculatedHertz()const;
	bool getMusicType()const;
	bool getIfMute()const;
	void DetectMusicTypeChange();
	void DetectIfMute();
	void MakeWhiteNoise(const double& kDuration);
	void MakeClusters(const double& kDuration);

private:
	//��ʼ��ʮ�������У�������C��ʼ��ʮ���������м���Ϊ0��
	double TwelveToneSeries[13]=
	{ 261.63,277.18,293.66,311.13,329.63,349.23,0,369.99,392.00,415.30,440.00,466.16,493.88 };
	std::vector<int> HeightDistribution;
	bool MusicTypeFlag;// true��Ӧʮ��������ģʽ��false��Ӧ΢�ְ�����ģʽ
	bool IfMute;// ����ģʽ
	double CalculatedHertz;// �������������ڲ���ʹ�ü���ͷ�������
	
	//Ϊ��ʵ��MakeClusters()���õ��ı�������ΪWindows API ��ر������ʲ����ձ���Ŀ�������ֵ�������ʽ
	WAVEFORMATEX wfx;
	HWAVEOUT hWaveOut;
	WAVEHDR header;
	MMRESULT result;
};
#endif // DISCJOCKEY_H