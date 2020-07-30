#include <MMSystem.h> 
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
	void startPlay(void);
	void stopPlay(void);
	void startSpeak(void);
	void stopSpeak(void);
	void playFile(void);	
private:
	static DWORD CALLBACK socketThread(LPVOID); 
	static DWORD CALLBACK playerThread(LPVOID);
	static DWORD CALLBACK recordThread(LPVOID); 
	static DWORD CALLBACK MicCallBack(HWAVEIN hWaveIn,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2);
private:
	static const int mPort = 18183;	

	char *recv_buf;
	char *send_buf;
	
	static const int PCM_IN_RATE = 8000;  //话筒采样率
	static const int OUT_BUF_MAX = 8;
	static const int OUT_BUF_SIZE = 4096;
	char *outBuf[OUT_BUF_SIZE];

	static const int PCM_OUT_RATE = 16000;  //播放采样率 
	static const int IN_BUF_MAX = 2;
	static const int IN_BUF_SIZE = 320;	
	char *inBuf[IN_BUF_SIZE];

    SOCKET sock_lis;
	SOCKET sock_cli;

	WAVEFORMATEX pOutFormat; 
	WAVEHDR      WaveOutHdr[OUT_BUF_MAX]; 
	HWAVEOUT     hWaveOut;
	WAVEFORMATEX pInFormat; 
	WAVEHDR      WaveInHdr[IN_BUF_SIZE]; 	 
	HWAVEIN      hWaveIn; 

	int send_count;
	int is_stop;
	int is_speak;
	int recv_fail_count;

	FILE *inFile;

public:
	HANDLE m_socketThread;  
	HANDLE m_playerThread;  
	HANDLE m_recordThread;  
};

