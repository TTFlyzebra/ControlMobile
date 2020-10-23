#include "SDL.h"
#include <queue>
#include <Mmsystem.h>
#pragma comment( lib,"winmm.lib" )
#pragma once

struct yuvFrame
{
	u_char *yuv;
};

class SDLWindow
{
public:
	SDLWindow(void);
	~SDLWindow(void);
	void init(CWnd *pCwnd, int width, int height);
	void release();
	void pushYUV(u_char *yuv);
	void upVideoYUV();

private:
	SDL_Window * pWindow;
	SDL_Texture * pTexture;
	SDL_Rect sdlRT;
	SDL_Rect dstRT;
	int iPitch;
	SDL_Renderer * pRender;
	SDL_RendererInfo info;
	CWnd *mPtr;

	CWnd *pCwnd; 
	int width; 
	int height;
	HANDLE pid_sdlkeyevent;  
	static DWORD CALLBACK sdlKeyEvent(LPVOID);
	DWORD SDLWindow::start();

	HANDLE pid_playthread;  
	static DWORD CALLBACK playThread(LPVOID);
	DWORD SDLWindow::playYUV();

	std::queue <u_char *> yuvList;
	CRITICAL_SECTION lock;
	DWORD lastTime;

	bool isStop;
};

