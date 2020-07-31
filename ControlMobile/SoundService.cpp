#include "stdafx.h"
#include "SoundService.h"
#include "ControlMobileDlg.h"
#include "protocol.h"


SoundService::SoundService(void)
{
	is_stop = 1;
	is_speak = 0;

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


void SoundService::startPlay(void)
{	
	if(is_stop != 0){
		is_stop = 0;
		send_count = 0;
		m_playerThread = CreateThread(NULL, 0, &SoundService::socketThread, this, CREATE_SUSPENDED, NULL);  
		if (NULL!= m_playerThread) {  
		     ResumeThread(m_playerThread);  
		}
	}
}

void SoundService::stopPlay(void)
{	
	is_stop = 1;	
	//hWaveIn = NULL;
	waveInStop(hWaveIn);
	waveInReset(hWaveIn);
	waveInClose(hWaveIn);
	closesocket(sock_cli);
	closesocket(sock_lis);	
}

DWORD CALLBACK SoundService::playerThread(LPVOID lp)
{	
	TRACE("playerThread start. \n"); 
	SoundService *mPtr=(SoundService *)lp;
	int recvLen = 0;
	int i = 0;
	MMRESULT     result; 
	//PlaySound("D:\\res\\music\\HHG.wav"), NULL, SND_FILENAME | SND_ASYNC);	
	result = waveOutOpen(&mPtr->hWaveOut, WAVE_MAPPER,&mPtr->pOutFormat,0L, 0L, WAVE_FORMAT_DIRECT); 
	if (result) 
	{ 
	    TRACE("Failed to open waveform output device."); 
		return -1;
	}
	int recv_fail_count = 0;
	//int j = 0;
	FILE *saveFile = fopen("D:\\res\\music\\record2s16k.pcm","wb");
	while (mPtr->is_stop == 0)	{
		memset(mPtr->recv_buf,0,5016);
		int recvLen = recv(mPtr->sock_cli, mPtr->recv_buf, 5016, 0);
		//TRACE("read recvLen=%d, number=%d. sokcet=%d.\n",recvLen,j,mPtr->sock_cli);
		if(recv_fail_count>10){
			closesocket(mPtr->sock_cli);
			TRACE("playerThread exit. \n"); 
			return -1;
		}
        if(recvLen < 6 ) {
			if(recvLen<0){
				recv_fail_count++;	
			}
			break;
		}
		recv_fail_count=0;
		long allLen = ((mPtr->recv_buf[2]<<24)&0xFF000000)+((mPtr->recv_buf[3]<<16)&0xFF0000)+((mPtr->recv_buf[4]<<8)&0xFF00)+mPtr->recv_buf[5];
		//TRACE("allLen=%d. sokcet=%d.\n",allLen,mPtr->sock_cli);
		while (recvLen<(allLen+4)){
			int ret = recv(mPtr->sock_cli, mPtr->recv_buf+recvLen, (allLen+4-recvLen), 0);
			if(ret<0){
				recv_fail_count++;	
			}else{
				recv_fail_count=0;				
				recvLen+=ret;
			}
			if(recv_fail_count>10){
				closesocket(mPtr->sock_cli);
				TRACE("playerThread exit. \n"); 
				return -1;
			}
		}

		while(recvLen>=6){
			if((mPtr->recv_buf[0]==(char)0x7e)&&(mPtr->recv_buf[1]==(char)0xa5)&&(mPtr->recv_buf[8]==(char)0x04)&&(mPtr->recv_buf[9]==(char)0x4a)) {
				TRACE("recv start play.\n");	
				int len = sizeof(PC_START_PLAY);
				if(recvLen-len>0){
					memmove(mPtr->recv_buf,mPtr->recv_buf+len,recvLen-len);
				}
				recvLen-=len;
				//TODO:
			}else if((mPtr->recv_buf[0]==(char)0x7e)&&(mPtr->recv_buf[1]==(char)0xa5)&&(mPtr->recv_buf[8]==(char)0x04)&&(mPtr->recv_buf[9]==(char)0x4b)) {
				int dataLen = ((mPtr->recv_buf[10]<<24)&0xFF000000)+((mPtr->recv_buf[11]<<16)&0xFF0000)+((mPtr->recv_buf[12]<<8)&0xFF00)+mPtr->recv_buf[13];
				//TRACE("recv play data, dataLen=%d\n",dataLen);
				memcpy(mPtr->outBuf[i],mPtr->recv_buf+14,dataLen);
				fwrite(mPtr->outBuf[i],1,dataLen,saveFile);
				mPtr->WaveOutHdr[i].lpData = mPtr->outBuf[i];
				mPtr->WaveOutHdr[i].dwBufferLength = dataLen;
				mPtr->WaveOutHdr[i].dwUser = 0L;
				mPtr->WaveOutHdr[i].dwFlags = 0L;
				mPtr->WaveOutHdr[i].dwLoops = 0L;
				waveOutPrepareHeader(mPtr->hWaveOut, &mPtr->WaveOutHdr[i], sizeof(WAVEHDR));
				waveOutWrite(mPtr->hWaveOut, &mPtr->WaveOutHdr[i], sizeof(WAVEHDR));
				i++;	
				if(i>=OUT_BUF_MAX) i = 0;
				int len = dataLen+sizeof(PC_PLAY_DATA);
				if(recvLen-len>0){
					memmove(mPtr->recv_buf,mPtr->recv_buf+len,recvLen-len);
				}
				recvLen-=len;
			}else if((mPtr->recv_buf[0]==(char)0x7e)&&(mPtr->recv_buf[1]==(char)0xa5)&&(mPtr->recv_buf[8]==(char)0x04)&&(mPtr->recv_buf[9]==(char)0x4c)) {
				TRACE("recv pause play.\n");	
				int len = sizeof(PC_PAUSE_PLAY);
				if(recvLen-len>0){
					memmove(mPtr->recv_buf,mPtr->recv_buf+len,recvLen-len);
				}
				recvLen-=len;
				//TODO:
			}else if((mPtr->recv_buf[0]==(char)0x7e)&&(mPtr->recv_buf[1]==(char)0xa5)&&(mPtr->recv_buf[8]==(char)0x04)&&(mPtr->recv_buf[9]==(char)0x4d)) {
				TRACE("recv stop play.\n");	
				int len = sizeof(PC_STOP_PLAY);
				if(recvLen-len>0){
					memmove(mPtr->recv_buf,mPtr->recv_buf+len,recvLen-len);
				}
				recvLen-=len;
				//TODO:
			}else if((mPtr->recv_buf[0]==(char)0x7e)&&(mPtr->recv_buf[1]==(char)0xa5)&&(mPtr->recv_buf[8]==(char)0x04)&&(mPtr->recv_buf[9]==(char)0x50)) {
				TRACE("recv open speak.\n");	
				int len = sizeof(PC_OPEN_SPEAK);
				if(recvLen-len>0){
					memmove(mPtr->recv_buf,mPtr->recv_buf+len,recvLen-len);
				}
				recvLen-=len;
				//TODO:
			}else if((mPtr->recv_buf[0]==(char)0x7e)&&(mPtr->recv_buf[1]==(char)0xa5)&&(mPtr->recv_buf[8]==(char)0x04)&&(mPtr->recv_buf[9]==(char)0x52)) {
				TRACE("recv close speak.\n");	
				int len = sizeof(PC_CLOSE_SPEAK);
				if(recvLen-len>0){
					memmove(mPtr->recv_buf,mPtr->recv_buf+len,recvLen-len);
				}
				recvLen-=len;
				//TODO:
			}else{
				recvLen = 0;
			}

			if(recvLen>0){
				while (recvLen<6){
					int ret = recv(mPtr->sock_cli, mPtr->recv_buf+recvLen, (6-recvLen), 0);
					TRACE("1 allLen=%d. recvLen=%d. ret=%d\n",allLen,recvLen,ret);
					if(ret<0){
						recv_fail_count++;	
					}else{
						recv_fail_count=0;
						TRACE("2 allLen=%d. recvLen=%d. ret=%d...\n",allLen,recvLen,ret);
						recvLen+=ret;
					}
					if(recv_fail_count>10){
						closesocket(mPtr->sock_cli);
						TRACE("playerThread exit. \n"); 
						return -1;
					}
				}
				
				allLen = ((mPtr->recv_buf[2]<<24)&0xFF000000)+((mPtr->recv_buf[3]<<16)&0xFF0000)+((mPtr->recv_buf[4]<<8)&0xFF00)+mPtr->recv_buf[5];
				//TRACE("allLen=%d. sokcet=%d.\n",allLen,mPtr->sock_cli);
				while (recvLen<(allLen+4)){
					int ret = recv(mPtr->sock_cli, mPtr->recv_buf+recvLen, (allLen+4-recvLen), 0);
					TRACE("3 allLen=%d. recvLen=%d. ret=%d\n",allLen,recvLen,ret);
					if(ret<0){
						recv_fail_count++;	
					}else{
						recv_fail_count=0;
						TRACE("4 allLen=%d. recvLen=%d. ret=%d\n",allLen,recvLen,ret);
						recvLen+=ret;
					}
					if(recv_fail_count>10){
						closesocket(mPtr->sock_cli);
						TRACE("playerThread exit. \n"); 
						return -1;
					}
				}
			}
		}
	}	

	fclose(saveFile);
		
	for(int n = 0;i<OUT_BUF_MAX;i++){
		while(waveOutUnprepareHeader(mPtr->hWaveOut,&mPtr->WaveOutHdr[i],sizeof(WAVEHDR)) == WAVERR_STILLPLAYING){
			TRACE("waveOutUnprepareHeader WAVERR_STILLPLAYING. \n"); 
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

DWORD CALLBACK SoundService::recordThread(LPVOID lp)
{
	TRACE("recordThread start. \n"); 
	SoundService *mPtr=(SoundService *)lp;	
	MMRESULT     result; 
	
	int n=waveInGetNumDevs();
	TRACE("waveInGetNumDevs %d. \n",n); 
	
	result = waveInOpen(&mPtr->hWaveIn, WAVE_MAPPER, &mPtr->pInFormat,(DWORD)MicCallBack, (DWORD)lp, CALLBACK_FUNCTION);
	if (result!= MMSYSERR_NOERROR) 
	{ 
	    TRACE("Failed to open waveform input device."); 
		return -1;
	}
	for(int i=0;i<IN_BUF_MAX;i++){
		mPtr->WaveInHdr[i].lpData = mPtr->inBuf[i];
		mPtr->WaveInHdr[i].dwBufferLength = IN_BUF_SIZE;
		mPtr->WaveInHdr[i].dwBytesRecorded = 0;
		mPtr->WaveInHdr[i].dwUser = 0L;
		mPtr->WaveInHdr[i].dwFlags = 0L;
		mPtr->WaveInHdr[i].dwLoops = 0L;
		mPtr->WaveInHdr[i].lpNext = NULL;
		mPtr->WaveInHdr[i].reserved = 0;
		waveInPrepareHeader(mPtr->hWaveIn, &mPtr->WaveInHdr[i], sizeof(WAVEHDR));		
		waveInAddBuffer(mPtr->hWaveIn, &mPtr->WaveInHdr[i], sizeof(WAVEHDR) );		
	}
	waveInStart(mPtr->hWaveIn);
	TRACE("recordThread exit. \n"); 
    return 0;
}

DWORD SoundService::MicCallBack(HWAVEIN hWaveIn,UINT uMsg,DWORD lp,DWORD dw1,DWORD dw2)
{
	//TRACE("MicCallBack start.%d, %d\n"),dw1,dw2); 
	SoundService *mPtr=(SoundService *)lp;
	PWAVEHDR pWaveInHdr =(PWAVEHDR)dw1;
	switch(uMsg)
	{
	case MM_WIM_OPEN:
		TRACE("MM_WIM_OPEN.\n"); 
		break;
	case MM_WIM_DATA:
		//TRACE("MM_WIM_DATA.\n"); 
		if(mPtr->is_stop==0 && mPtr->is_speak == 1) {
			int sendLen=send(mPtr->sock_cli, pWaveInHdr->lpData, IN_BUF_SIZE, 0);
			//fwrite(pWaveInHdr->lpData,1,IN_BUF_SIZE,mPtr->inFile);
			TRACE("send sendLen=%d, number=%d, sokcet=%d.\n",sendLen,mPtr->send_count,mPtr->sock_cli);
			mPtr->send_count++;
			waveInAddBuffer(hWaveIn, pWaveInHdr, sizeof(WAVEHDR) );
		}
		break;
	case MM_WIM_CLOSE:
		TRACE("MM_WIM_CLOSE.\n"); 
		waveInReset(hWaveIn);
	    waveInClose(hWaveIn);
		break;
	default:
		break;
	}
	//TRACE("MicCallBack exit.\n"); 
	return 0;
}

void SoundService::startSpeak(void)
{	
	//inFile = fopen("D:\\res\\music\\temp.pcm","wb+");
	is_speak = 1;
	TRACE("recordThread start. \n"); 
	MMRESULT     result; 
	
	int n=waveInGetNumDevs();
	TRACE("waveInGetNumDevs %d. \n",n); 
	
	result = waveInOpen(&hWaveIn, WAVE_MAPPER, &pInFormat,(DWORD)MicCallBack, (DWORD)this, CALLBACK_FUNCTION);
	if (result!= MMSYSERR_NOERROR) { 
	    TRACE("Failed to open waveform input device."); 
	}else{
		for(int i=0;i<IN_BUF_MAX;i++){
			WaveInHdr[i].lpData = inBuf[i];
			WaveInHdr[i].dwBufferLength = IN_BUF_SIZE;
			WaveInHdr[i].dwBytesRecorded = 0;
			WaveInHdr[i].dwUser = 0L;
			WaveInHdr[i].dwFlags = 0L;
			WaveInHdr[i].dwLoops = 1L;
			WaveInHdr[i].lpNext = NULL;
			WaveInHdr[i].reserved = 0;
			waveInPrepareHeader(hWaveIn, &WaveInHdr[i], sizeof(WAVEHDR));		
			waveInAddBuffer(hWaveIn, &WaveInHdr[i], sizeof(WAVEHDR) );		
		}
		waveInStart(hWaveIn);
	}
}

char b[8];
void SoundService::stopSpeak(void)
{	
	//fclose(inFile);
	char a[4] = {'a','c','c','d'};
	TRACE("memcpy test a=%s, b=%s.",a,b); 
	memcpy(b,a,4);
	memcpy(b+4,a+2,2);
	memcpy(b+6,a+1,2);
	TRACE("memcpy test a=%s, b=%s.",a,b); 
	if(is_speak == 1){
		is_speak = 0;
		waveInStop(hWaveIn);
		waveInReset(hWaveIn);
		waveInClose(hWaveIn);
	}
}


DWORD CALLBACK SoundService::socketThread(LPVOID lp)
{
	TRACE("socketThread start. \n");
	SoundService *mPtr=(SoundService *)lp;
	struct sockaddr_in sin;
	struct sockaddr_in remoteAddr;	

	mPtr->sock_lis = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mPtr->sock_lis == INVALID_SOCKET)
	{
		TRACE("socket error !");
		return -1;
	} 		
	sin.sin_family = AF_INET;
	sin.sin_port = htons(mPort);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(mPtr->sock_lis, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		TRACE("bind error !");
		return -1;
	}
	if (listen(mPtr->sock_lis, 5) == SOCKET_ERROR)
	{
		TRACE("listen error !");
		return -1;
	}	
	int nAddrlen = sizeof(remoteAddr);
	while (mPtr->is_stop == 0)
	{
		TRACE("accept client connect, socke_cli=%d.\n",mPtr->sock_cli);
		mPtr->sock_cli = accept(mPtr->sock_lis, (SOCKADDR *)&remoteAddr, &nAddrlen);
		TRACE("accept socke_cli=%d.\n",mPtr->sock_cli);
		if(mPtr->sock_cli != INVALID_SOCKET){
			if(mPtr->m_playerThread != NULL){
				mPtr->m_playerThread = CreateThread(NULL, 0, &SoundService::playerThread, lp, CREATE_SUSPENDED, NULL);  
				if (NULL!= mPtr->m_playerThread) {  
				     ResumeThread(mPtr->m_playerThread);  
				}			
			}
			//if(mPtr->m_recordThread!=NULL){
			//	mPtr->m_recordThread = CreateThread(NULL, 0, &SoundService::recordThread, lp, CREATE_SUSPENDED, NULL);  
			//	if (NULL!= mPtr->m_recordThread) {  
			//	     ResumeThread(mPtr->m_recordThread);  
			//	}
			//}
		}
	}	
	TRACE("socketThread exit. \n"); 
	return 0;
}

void SoundService::playFile(void)
{
	TRACE("recordThread start. \n"); 		
	FILE *file;		
	char readBuf[IN_BUF_SIZE];		
	SoundService *mPtr=this;		
	file = fopen("D:\\res\\music\\HHG8k.pcm","rb");		   
	int i = 0;	  
	while (mPtr->is_stop == 0){		
		int readLen = fread(readBuf,1,IN_BUF_SIZE,file);	
		if(readLen>0){	
			int sendLen=send(mPtr->sock_cli, readBuf, readLen, 0);	
 			TRACE("send readLen=%d, number=%d \n",readLen,i);	
		}else{	    TRACE("recordThread start. \n"); 
			fseek(file,0,0);	
			TRACE("fseek 0. \n");		
		}	
 		i++;		
	}		
	fclose(file);	
 	TRACE("recordThread exit. \n"); 		
}
