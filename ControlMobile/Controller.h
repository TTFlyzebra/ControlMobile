#include <winsock2.h>
#include <windows.h>
#include "SDLWindow.h"
#pragma comment(lib,"ws2_32.lib")
#pragma once
class Controller
{
public:
	Controller(void);
	~Controller(void);
	void start();
	void stop();
private:

	SOCKET sock_lis;

	bool isStop;
	HANDLE m_socketThread;  
	static DWORD CALLBACK socketThread(LPVOID); 

	HANDLE m_sendThread;  
	static DWORD CALLBACK sendThread(LPVOID);
	
};

