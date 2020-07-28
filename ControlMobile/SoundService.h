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
	SoundService(void);
	~SoundService(void);
	void start();
	void stop();
private:
	static DWORD CALLBACK socketThread(LPVOID); 
	static DWORD CALLBACK playerThread(LPVOID);
	static DWORD CALLBACK recordThread(LPVOID); 
private:
	static const int PCM_RATE = 8000;  //刺激率 
	static const int PCM_SAMPLE_RATE = 44100;  //采样率 	
	static const int OUT_BUF_MAX = 4;
	static const int OUT_BUF_SIZE = 4096;
	static const int IN_BUF_MAX = 4;
	static const int IN_BUF_SIZE = 320;
	static const int SEND_BUF_SIZE = 320;
	char *recvBuf[OUT_BUF_SIZE];
	char *sendBuf[IN_BUF_SIZE];

    SOCKET slisten;
	SOCKET sock_cli;
	WAVEFORMATEX pOutFormat; 
	WAVEHDR WaveOutHdr[OUT_BUF_MAX]; 

	WAVEFORMATEX pInFormat; 
	WAVEHDR WaveIntHdr[OUT_BUF_MAX]; 

	int is_stop;
public:
	HANDLE m_socketThread;  
	HANDLE m_playerThread;  
	HANDLE m_recordThread;  
};

