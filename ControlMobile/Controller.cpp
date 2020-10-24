#include "stdafx.h"
#include "Controller.h"
#define EVENT_STOP (SDL_USEREVENT + 1)

bool IsSocketClosed(SOCKET clientSocket)  
{  
	bool ret = false;  
	HANDLE closeEvent = WSACreateEvent();  
	WSAEventSelect(clientSocket, closeEvent, FD_CLOSE);  
	DWORD dwRet = WaitForSingleObject(closeEvent, 0);
	if(dwRet == WSA_WAIT_EVENT_0)  
		ret = true;  
	else if(dwRet == WSA_WAIT_TIMEOUT)  
		ret = false; 
	WSACloseEvent(closeEvent);  
	return ret;  
} 

Controller::Controller(void)
{
}

Controller::~Controller(void)
{
}

void Controller::start()
{
	isStop = false;
	m_socketThread = CreateThread(NULL, 0, &Controller::socketThread, this, CREATE_SUSPENDED, NULL);  
	if (NULL!= m_socketThread) {  
		ResumeThread(m_socketThread);  
	}
}

DWORD CALLBACK Controller::socketThread(LPVOID lp)
{
	TRACE("NetWorkService socketThread start. \n");
	Controller *mPtr=(Controller *)lp;
	struct sockaddr_in sin;
	struct sockaddr_in remoteAddr;
	mPtr->sock_lis = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mPtr->sock_lis == INVALID_SOCKET)
	{
		TRACE("socket error !");
		return -1;
	} 		
	sin.sin_family = AF_INET;
	sin.sin_port = htons(9008);
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
	while (!mPtr->isStop)
	{
		SOCKET sock_cli = accept(mPtr->sock_lis, (SOCKADDR *)&remoteAddr, &nAddrlen);
		TRACE("NetWorkService accept socke_cli=%d.\n",sock_cli);
		if(sock_cli != INVALID_SOCKET){
			mPtr->m_sendThread = CreateThread(NULL, 0, &Controller::sendThread, &sock_cli, CREATE_SUSPENDED, NULL);  
			if (NULL!= mPtr->m_sendThread) {  
				ResumeThread(mPtr->m_sendThread);  
			}	
		}
	}	
	TRACE("socketThread exit. \n"); 
	return 0;
}

DWORD CALLBACK Controller::sendThread(LPVOID lp)
{
	SOCKET* pTmp = (SOCKET*)lp;
    SOCKET  m_socket = (SOCKET)(*pTmp);
	SDL_Event event;
	while (SDL_WaitEvent(&event)) {
		switch (event.type) {
		case EVENT_STOP:
			TRACE("SDLWindow stop\n");
			return 0;
		case SDL_QUIT:
			TRACE("SDL_WaitEvent SDL_QUIT\n");
			return 0;
		case SDL_WINDOWEVENT:
			break;
		case SDL_TEXTINPUT:
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			break;
		case SDL_MOUSEMOTION:
			break;
		case SDL_MOUSEWHEEL:
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			break;
		case SDL_FINGERMOTION:
		case SDL_FINGERDOWN:
		case SDL_FINGERUP:
			break;
		}
		int len = send(m_socket,"xxxx",4,0);
		TRACE("send len=%d\n", len);
		bool ret = IsSocketClosed(m_socket);
		TRACE("send IsSocketClosed ret=%d\n", ret);
	}
	return 0;
}


void Controller::stop()
{
	isStop = true;
	closesocket(sock_lis);
}