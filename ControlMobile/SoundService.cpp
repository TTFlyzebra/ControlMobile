#include "stdafx.h"
#include "SoundService.h"
#include "ControlMobileDlg.h"


SoundService::SoundService(HWND hwnd)
{
	mHwnd = hwnd;
	is_stop = 1;

	for(int i=0;i<OUT_BUF_MAX;i++){
		outBuf[i] = new char[OUT_BUF_SIZE];
	}
	for(int i=0;i<IN_BUF_MAX;i++){
		inBuf[i] = new char[IN_BUF_SIZE];
	}

	//初始化WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		TRACE("WSAStartup error !");
	}

	pOutFormat.wFormatTag=WAVE_FORMAT_PCM;	//simple,uncompressed format 
	pOutFormat.nChannels = 2;//1=mono, 2=stereo 
	pOutFormat.nSamplesPerSec = PCM_OUT_RATE; // 44100 
	pOutFormat.nAvgBytesPerSec = PCM_OUT_RATE * 2 * 16 / 8; 	// = nSamplesPerSec * n.Channels * wBitsPerSample/8 
	pOutFormat.wBitsPerSample = 16;	//16 for high quality, 8 for telephone-grade 
	pOutFormat.nBlockAlign = 2 * 16 / 8; // = n.Channels * wBitsPerSample/8 
	pOutFormat.cbSize=0; 
	
	pInFormat.wFormatTag = WAVE_FORMAT_PCM;	//simple,uncompressed format 
	pInFormat.nChannels = 1;//1=mono, 2=stereo 
	pInFormat.nSamplesPerSec = PCM_IN_RATE; // 8000 
	pInFormat.nAvgBytesPerSec = PCM_IN_RATE * 1 * 16 / 8; 	// = nSamplesPerSec * n.Channels * wBitsPerSample/8 
	pInFormat.wBitsPerSample = 16;	//16 for high quality, 8 for telephone-grade 
	pInFormat.nBlockAlign = 1 * 16 / 8; // = n.Channels * wBitsPerSample/8 
	pInFormat.cbSize = 0; 

	////内容 
	//for (int i=0;i<NUMPTS;i++) 
	//{ 
	//    waveOut[i] = (short int)ceil(sin(2*3.1415926*rate*i/NUMPTS)*20000); 
	//} 
}


SoundService::~SoundService(void)
{	
	for(int i=0;i<OUT_BUF_MAX;i++){
		delete [] outBuf[OUT_BUF_SIZE];
	}
	for(int i=0;i<IN_BUF_MAX;i++){
		delete [] inBuf[IN_BUF_SIZE];
	}

	WSACleanup();//释放资源的操作
	TRACE("~SoundService");
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
	//hWaveIn = NULL;
	waveInStop(hWaveIn);
	waveInReset(hWaveIn);
	waveInClose(hWaveIn);
	closesocket(sock_cli);
	closesocket(slisten);	
}


DWORD CALLBACK SoundService::socketThread(LPVOID lp){
	TRACE("socketThread start. \n");
	SoundService *mPtr=(SoundService *)lp;
	struct sockaddr_in sin;
	struct sockaddr_in remoteAddr;	

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
	SoundService *mPtr=(SoundService *)lp;
	int recvLen = 0;
	int i = 0;
	MMRESULT     result; 
	//PlaySound(_T("D:\\res\\music\\HHG.wav"), NULL, SND_FILENAME | SND_ASYNC);	
	result = waveOutOpen(&mPtr->hWaveOut, WAVE_MAPPER,&mPtr->pOutFormat,0L, 0L, WAVE_FORMAT_DIRECT); 
	if (result) 
	{ 
	    TRACE(_T("Failed to open waveform output device.")); 
		return -1;
	}
	while (mPtr->is_stop == 0)
	{
		//memset(recvBuf[i],0,OUT_BUF_SIZE);
		recvLen = recv(mPtr->sock_cli,mPtr->outBuf[i],OUT_BUF_SIZE,0);
		//TRACE(_T("play read len=%d, number=%d. \n"),recvLen, j); 
		if(recvLen>0){
			mPtr->WaveOutHdr[i].lpData = mPtr->outBuf[i];
			mPtr->WaveOutHdr[i].dwBufferLength = OUT_BUF_SIZE;
			mPtr->WaveOutHdr[i].dwUser = 0L;
			mPtr->WaveOutHdr[i].dwFlags = 0L;
			mPtr->WaveOutHdr[i].dwLoops = 0L;
			waveOutPrepareHeader(mPtr->hWaveOut, &mPtr->WaveOutHdr[i], sizeof(WAVEHDR));
			waveOutWrite(mPtr->hWaveOut, &mPtr->WaveOutHdr[i], sizeof(WAVEHDR));
			//WaitForSingleObject(hWaveOut, INFINITE);
			i++;
		}
		if(i>=OUT_BUF_MAX) i = 0;
	}
	for(int n = 0;i<OUT_BUF_MAX;i++){
		while(waveOutUnprepareHeader(mPtr->hWaveOut,&mPtr->WaveOutHdr[i],sizeof(WAVEHDR)) == WAVERR_STILLPLAYING){
			TRACE(_T("waveOutUnprepareHeader WAVERR_STILLPLAYING. \n")); 
			Sleep(500);
		}
	}
	Sleep(500);
	waveOutReset(mPtr->hWaveOut);
    waveOutClose(mPtr->hWaveOut);
    mPtr->hWaveOut = NULL;	
	TRACE("playerThread exit. \n"); 
	return 0;
}

DWORD CALLBACK SoundService::recordThread(LPVOID lp){
    TRACE("recordThread start. \n"); 
	SoundService *mPtr=(SoundService *)lp;	
	MMRESULT     result; 

	int n=waveInGetNumDevs();
	TRACE("waveInGetNumDevs %d. \n",n); 

	mPtr->i_in_count = 0;
	result = waveInOpen(&mPtr->hWaveIn, WAVE_MAPPER, &mPtr->pInFormat,(DWORD)MicCallBack, (DWORD)lp, CALLBACK_FUNCTION);
	if (result!= MMSYSERR_NOERROR) 
	{ 
	    TRACE(_T("Failed to open waveform input device.")); 
		return -1;
	}
	mPtr->WaveInHdr[mPtr->i_in_count].lpData = mPtr->inBuf[mPtr->i_in_count];
	mPtr->WaveInHdr[mPtr->i_in_count].dwBufferLength = IN_BUF_SIZE;
	mPtr->WaveInHdr[mPtr->i_in_count].dwBytesRecorded = 0;
	mPtr->WaveInHdr[mPtr->i_in_count].dwUser = 0L;
	mPtr->WaveInHdr[mPtr->i_in_count].dwFlags = 0L;
	mPtr->WaveInHdr[mPtr->i_in_count].dwLoops = 0L;
	mPtr->WaveInHdr[mPtr->i_in_count].lpNext = NULL;
	mPtr->WaveInHdr[mPtr->i_in_count].reserved = 0;
	waveInPrepareHeader(mPtr->hWaveIn, &mPtr->WaveInHdr[mPtr->i_in_count], sizeof(WAVEHDR));		
	waveInAddBuffer(mPtr->hWaveIn, &mPtr->WaveInHdr[mPtr->i_in_count], sizeof(WAVEHDR) );
	waveInStart(mPtr->hWaveIn);
	mPtr->i_in_count++;
	if(mPtr->i_in_count>=IN_BUF_MAX) mPtr->i_in_count = 0;
	waveInStart(mPtr->hWaveIn);
	TRACE("recordThread exit. \n"); 
	return 0;
}

DWORD SoundService::MicCallBack(HWAVEIN hWaveIn,UINT uMsg,DWORD lp,DWORD dw1,DWORD dw2)
{
	TRACE(_T("MicCallBack start.\n")); 
	SoundService *mPtr=(SoundService *)lp;
	switch(uMsg)
	{
	case MM_WIM_OPEN:
		TRACE(_T("MM_WIM_OPEN.\n")); 
		break;
	case MM_WIM_DATA:
		TRACE(_T("MM_WIM_DATA.\n")); 
		if(mPtr->is_stop==0) {
			int num = (mPtr->i_in_count-1+IN_BUF_MAX)%IN_BUF_MAX;
			int sendLen=send(mPtr->sock_cli, mPtr->inBuf[num], IN_BUF_SIZE, 5000);
			TRACE("send sendLen=%d, number=%d \n",sendLen,mPtr->i_in_count);
			mPtr->WaveInHdr[mPtr->i_in_count].lpData = mPtr->inBuf[mPtr->i_in_count];
			mPtr->WaveInHdr[mPtr->i_in_count].dwBufferLength = IN_BUF_SIZE;
			mPtr->WaveInHdr[mPtr->i_in_count].dwBytesRecorded = 0;
			mPtr->WaveInHdr[mPtr->i_in_count].dwUser = 0L;
			mPtr->WaveInHdr[mPtr->i_in_count].dwFlags = 0L;
			mPtr->WaveInHdr[mPtr->i_in_count].dwLoops = 0L;
			mPtr->WaveInHdr[mPtr->i_in_count].lpNext = NULL;
			mPtr->WaveInHdr[mPtr->i_in_count].reserved = 0;
			waveInPrepareHeader(mPtr->hWaveIn, &mPtr->WaveInHdr[mPtr->i_in_count], sizeof(WAVEHDR));		
			waveInAddBuffer(mPtr->hWaveIn, &mPtr->WaveInHdr[mPtr->i_in_count], sizeof(WAVEHDR) );
			mPtr->i_in_count++;
			if(mPtr->i_in_count>=IN_BUF_MAX) mPtr->i_in_count = 0;
		}
		break;
	case MM_WIM_CLOSE:
		TRACE(_T("MM_WIM_CLOSE.\n")); 
		waveInReset(hWaveIn);
	    waveInClose(hWaveIn);
		break;
	default:
		break;
	}
	TRACE(_T("MicCallBack exit.\n")); 
	return 0;
}
