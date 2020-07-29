#include <MMSystem.h> 
#include "math.h"
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"winmm.lib") 
#pragma once
class SoundService
{
public:
	SoundService(HWND hwnd);
	~SoundService(void);
	void start();
	void stop();
private:
	static DWORD CALLBACK socketThread(LPVOID); 
	static DWORD CALLBACK playerThread(LPVOID);
	static DWORD CALLBACK recordThread(LPVOID); 
	static DWORD CALLBACK MicCallBack(HWAVEIN hWaveIn,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2);
private:
	static const int PCM_IN_RATE = 8000;  //刺激率 
	static const int PCM_OUT_RATE = 44100;  //采样率 	
	static const int OUT_BUF_MAX = 4;
	static const int OUT_BUF_SIZE = 4096;
	static const int IN_BUF_MAX = 4;
	static const int IN_BUF_SIZE = 320;
	char *outBuf[OUT_BUF_SIZE];
	char *inBuf[IN_BUF_SIZE];

    SOCKET slisten;
	SOCKET sock_cli;

	WAVEFORMATEX pOutFormat; 
	WAVEHDR      WaveOutHdr[OUT_BUF_MAX]; 
	HWAVEOUT     hWaveOut;

	WAVEFORMATEX pInFormat; 
	WAVEHDR      WaveInHdr[IN_BUF_SIZE]; 	 
	HWAVEIN      hWaveIn; 
	int i_in_count;

	int is_stop;
	HWND mHwnd;
public:
	HANDLE m_socketThread;  
	HANDLE m_playerThread;  
	HANDLE m_recordThread;  
};

