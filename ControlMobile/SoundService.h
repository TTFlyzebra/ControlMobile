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
	static const int RECV_BUF_MAX = 4;
	static const int RECV_BUF_SIZE = 4096;
	static const int SEND_BUF_SIZE = 1024;
	char *recvBuf[RECV_BUF_SIZE];

    SOCKET slisten;
	SOCKET sock_cli;
	WAVEFORMATEX pFormat; 
	WAVEHDR WaveOutHdr[RECV_BUF_MAX]; 
	int is_stop;
public:
	HANDLE m_socketThread;  
	HANDLE m_playerThread;  
	HANDLE m_recordThread;  
};

