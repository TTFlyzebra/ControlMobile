#include "stdafx.h"
#include "SoundService.h"


SoundService::SoundService(void)
{
	is_stop = 1;

	for(int i=0;i<OUT_BUF_MAX;i++){
		recvBuf[i] = new char[OUT_BUF_SIZE];
	}

	//初始化WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		TRACE("WSAStartup error !");
	}

	pOutFormat.wFormatTag=WAVE_FORMAT_PCM;	//simple,uncompressed format 
	pOutFormat.nChannels=2;//1=mono, 2=stereo 
	pOutFormat.nSamplesPerSec=PCM_SAMPLE_RATE; // 44100 
	pOutFormat.nAvgBytesPerSec=PCM_SAMPLE_RATE * 2; 	// = nSamplesPerSec * n.Channels * wBitsPerSample/8 
	pOutFormat.wBitsPerSample=16;	//16 for high quality, 8 for telephone-grade 
	pOutFormat.nBlockAlign=4; // = n.Channels * wBitsPerSample/8 
	pOutFormat.cbSize=0; 
	
	pOutFormat.wFormatTag=WAVE_FORMAT_PCM;	//simple,uncompressed format 
	pOutFormat.nChannels=2;//1=mono, 2=stereo 
	pOutFormat.nSamplesPerSec=PCM_SAMPLE_RATE; // 44100 
	pOutFormat.nAvgBytesPerSec=PCM_SAMPLE_RATE * 2; 	// = nSamplesPerSec * n.Channels * wBitsPerSample/8 
	pOutFormat.wBitsPerSample=16;	//16 for high quality, 8 for telephone-grade 
	pOutFormat.nBlockAlign=4; // = n.Channels * wBitsPerSample/8 
	pOutFormat.cbSize=0; 

	////内容 
	//for (int i=0;i<NUMPTS;i++) 
	//{ 
	//    waveOut[i] = (short int)ceil(sin(2*3.1415926*rate*i/NUMPTS)*20000); 
	//} 
}


SoundService::~SoundService(void)
{
	for(int i=0;i<OUT_BUF_MAX;i++){
		delete [] recvBuf[OUT_BUF_SIZE];
	}	

	WSACleanup();//释放资源的操作
}


void SoundService::start(void)
{
	if(is_stop!=0){
		is_stop = 0;
		m_playerThread = CreateThread(NULL, 0, &SoundService::socketThread, this, CREATE_SUSPENDED, NULL);  
		if (NULL!= m_playerThread) {  
		     ResumeThread(m_playerThread);  
		}
	}
}

void SoundService::stop(void)
{	
	is_stop = 1;	
	closesocket(sock_cli);
	closesocket(slisten);	
}


DWORD CALLBACK SoundService::socketThread(LPVOID lp){
	TRACE("socketThread start. \n");
	struct sockaddr_in sin;
	struct sockaddr_in remoteAddr;

	SoundService *mPtr=(SoundService *)lp;
	mPtr->slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mPtr->slisten == INVALID_SOCKET)
	{
		TRACE("socket error !");
		return -1;
	} 		
	sin.sin_family = AF_INET;
	sin.sin_port = htons(9001);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(mPtr->slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		TRACE("bind error !");
		return -1;
	}
	if (listen(mPtr->slisten, 5) == SOCKET_ERROR)
	{
		TRACE("listen error !");
		return -1;
	}	
	int nAddrlen = sizeof(remoteAddr);
	while (mPtr->is_stop == 0)
	{
		TRACE("accept client connect, socke_cli=%d.\n",mPtr->sock_cli);
		mPtr->sock_cli = accept(mPtr->slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		TRACE("accept socke_cli=%d.\n",mPtr->sock_cli);
		if(mPtr->m_playerThread != NULL){
			mPtr->m_playerThread = CreateThread(NULL, 0, &SoundService::playerThread, lp, CREATE_SUSPENDED, NULL);  
			if (NULL!= mPtr->m_playerThread) {  
			     ResumeThread(mPtr->m_playerThread);  
			}			
		}
		if(mPtr->m_recordThread!=NULL){
			mPtr->m_recordThread = CreateThread(NULL, 0, &SoundService::recordThread, lp, CREATE_SUSPENDED, NULL);  
			if (NULL!= mPtr->m_recordThread) {  
			     ResumeThread(mPtr->m_recordThread);  
			}
		}
	}	
	TRACE("socketThread exit. \n"); 
	return 0;
}

DWORD CALLBACK SoundService::playerThread(LPVOID lp){	
	TRACE("playerThread start. \n"); 
	int recvLen = 0;
	int i = 0;
	int j = 0;
	HWAVEOUT     hWaveOut; 
	MMRESULT     result; 
	SoundService *mPtr=(SoundService *)lp;
	//PlaySound(_T("D:\\res\\music\\HHG.wav"), NULL, SND_FILENAME | SND_ASYNC);	
	result = waveOutOpen(&hWaveOut, WAVE_MAPPER,&mPtr->pOutFormat,0L, 0L, WAVE_FORMAT_DIRECT); 
	if (result) 
	{ 
	    TRACE(_T("Failed to open waveform output device.")); 
		return -1;
	}
	while (mPtr->is_stop == 0)
	{
		//memset(recvBuf[i],0,OUT_BUF_SIZE);
		recvLen = recv(mPtr->sock_cli,mPtr->recvBuf[i],OUT_BUF_SIZE,0);
		//TRACE(_T("play read len=%d, number=%d. \n"),recvLen, j); 
		if(recvLen>0){
			mPtr->WaveOutHdr[i].lpData = mPtr->recvBuf[i];
			mPtr->WaveOutHdr[i].dwBufferLength = OUT_BUF_SIZE;
			mPtr->WaveOutHdr[i].dwUser = 0L;
			mPtr->WaveOutHdr[i].dwFlags = 0L;
			mPtr->WaveOutHdr[i].dwLoops = 0L;
			waveOutPrepareHeader(hWaveOut, &mPtr->WaveOutHdr[i], sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, &mPtr->WaveOutHdr[i], sizeof(WAVEHDR));
			//WaitForSingleObject(hWaveOut, INFINITE);
			i++;
		}
		j++;
		if(i>=OUT_BUF_MAX) i = 0;
	}
	for(int n = 0;i<OUT_BUF_MAX;i++){
		while(waveOutUnprepareHeader(hWaveOut,&mPtr->WaveOutHdr[i],sizeof(WAVEHDR)) == WAVERR_STILLPLAYING){
			TRACE(_T("waveOutUnprepareHeader WAVERR_STILLPLAYING. \n")); 
			Sleep(500);
		}
	}
	Sleep(500);
    waveOutClose(hWaveOut);
    hWaveOut = NULL;	
	TRACE("playerThread exit. \n"); 
	return 0;
}

DWORD CALLBACK SoundService::recordThread(LPVOID lp){
    TRACE("recordThread start. \n"); 
	FILE *file;
	char readBuf[SEND_BUF_SIZE];
	SoundService *mPtr=(SoundService *)lp;
	file = fopen("D:\\res\\music\\HHG8k.pcm","rb");	
	int i = 0;
	while (mPtr->is_stop == 0){
		int readLen = fread(readBuf,1,SEND_BUF_SIZE,file);
		if(readLen>0){
			int sendLen=send(mPtr->sock_cli, readBuf, readLen, 0);
			TRACE("send readLen=%d, number=%d \n",readLen,i);
		}else{
			fseek(file,0,0);
			TRACE("fseek 0. \n");
		}
		i++;
	}
	fclose(file);
	TRACE("recordThread exit. \n"); 
	return 0;
}
