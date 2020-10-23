#include "stdafx.h"
#include "SDLWindow.h"
#include "resource.h"


#define EVENT_STOP (SDL_USEREVENT + 1)
#define EVENT_UPYUV (SDL_USEREVENT + 2)


SDLWindow::SDLWindow(void)
{
	lastTime = 0;
	isStop = false;
	InitializeCriticalSection(&lock);
}


SDLWindow::~SDLWindow(void)
{
	DeleteCriticalSection(&lock);
}

void SDLWindow::init(CWnd *pCwnd, int width, int height)
{
	TRACE("SDLWindow create, width=%d, height=%d\n",width,height);
	this->pCwnd = pCwnd;
	this->width = width;
	this->height = height;
	if (SDL_Init(SDL_INIT_VIDEO)) {
		TRACE("Video is initialized.\n");
	} else {
		TRACE("Video is not initialized.\n");
	}	
	//用mfc窗口句柄创建一个sdl window
	pWindow = SDL_CreateWindowFrom((void *)(pCwnd->GetDlgItem(IDC_VIDEO)->GetSafeHwnd())); 
	TRACE( "SDL_CreateWindowFrom %s\n", SDL_GetError()); 

	sdlRT.w = width;
	sdlRT.h = height;
	sdlRT.x = 0;
	sdlRT.y = 0; 


	//计算yuv一行数据占的字节数

	int iWidth = 0;
	int iHeight = 0;
	SDL_GetWindowSize( pWindow, &iWidth, &iHeight );   
	dstRT.w = iWidth;
	dstRT.h = iHeight;
	dstRT.x = 0;
	dstRT.y = 0;
	TRACE("SDL_GetWindowSize, width=%d, height=%d\n",iWidth,iHeight);

	//获取当前可用画图驱动 window中有3个，第一个为d3d，第二个为opengl，第三个为software
	//int iii = SDL_GetNumRenderDrivers();
	//创建渲染器，第二个参数为选用的画图驱动，0代表d3d
	pRender = SDL_CreateRenderer( pWindow, 0, SDL_RENDERER_ACCELERATED );
	TRACE( "SDL_CreateRenderer %s\n", SDL_GetError()); 

	SDL_GetRendererInfo(pRender, &info);

	SDL_GetRenderDriverInfo(0, &info);    //d3d
	SDL_GetRenderDriverInfo(1, &info);    //opgl
	SDL_GetRenderDriverInfo(2, &info);    //software

	char szInfo[256] = {0};
	TRACE( "info.name = %s\n", info.name);
	TRACE( "SDL_GetRendererInfo %s\n", SDL_GetError());

	//创建纹理
	pTexture = SDL_CreateTexture( pRender,SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, width, height );   
	TRACE("SDLWindow create finish!\n");

	pid_sdlkeyevent = CreateThread(NULL, 0, &SDLWindow::sdlKeyEvent, this, CREATE_SUSPENDED, NULL);
	if (NULL!= pid_sdlkeyevent) {  
		ResumeThread(pid_sdlkeyevent);  
	}
	pid_playthread = CreateThread(NULL, 0, &SDLWindow::playThread, this, CREATE_SUSPENDED, NULL);
	if (NULL!= pid_playthread) {  
		ResumeThread(pid_playthread);  
	}
}

DWORD CALLBACK SDLWindow::sdlKeyEvent(LPVOID lp)
{
	SDLWindow *mPtr = (SDLWindow *)lp;
	return mPtr->start();
}

DWORD SDLWindow::start()
{
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
		case SDL_DROPFILE: 
			break;
		}
	}
	return 0;
}

void SDLWindow::pushYUV(u_char *yuv)
{	
	if(!isStop){
		EnterCriticalSection(&lock);
		yuvList.push(yuv);
		LeaveCriticalSection(&lock);	
	}
}

DWORD CALLBACK SDLWindow::playThread(LPVOID lp)
{
	SDLWindow *mPtr = (SDLWindow *)lp;
	return mPtr->playYUV();
}

DWORD SDLWindow::playYUV()
{
	while (!isStop){
		EnterCriticalSection(&lock);
		int size = yuvList.size();
		if(yuvList.empty()){
			LeaveCriticalSection(&lock); 
			continue;
		}
		u_char *yuv = yuvList.front();
		yuvList.pop();
		LeaveCriticalSection(&lock); 
		int ret = SDL_UpdateTexture( pTexture, NULL, yuv, width);
		//int e = SDL_UpdateYUVTexture(pTexture,  NULL, yuv, width,	yuv + width*height, width / 2, yuv + width*height +width*height/4, width / 2);
		SDL_RenderClear( pRender );
		SDL_RenderCopy( pRender, pTexture, NULL,NULL);
		SDL_RenderPresent( pRender );
		
		free(yuv);

		DWORD curretTime = timeGetTime();
		DWORD sleepTime = 1000/25 - (curretTime - lastTime);
		//TRACE("curretTime=%d,lastTime=%d,sleepTime=%d,size=%d\n",curretTime,lastTime,sleepTime,size);
		lastTime = curretTime;		
		if(size<8 && sleepTime>0 && sleepTime<(1000/24)){
			//TRACE("sleepTime = %d\n",sleepTime);
			Sleep(sleepTime);
		}
	}
	return 0;
}


void SDLWindow::release()
{
	isStop = true;
	SDL_Event stop_event;
	stop_event.type = EVENT_STOP;
    SDL_PushEvent(&stop_event);
	EnterCriticalSection(&lock);
	while(!yuvList.empty()){
		u_char *yuv = yuvList.front();
		free(yuv);
		yuvList.pop();
	}
	LeaveCriticalSection(&lock); 

	if ( pTexture != NULL )
	{
		SDL_DestroyTexture( pTexture );
		pTexture = NULL    ;
	}

	if ( pRender != NULL )
	{
		SDL_DestroyRenderer( pRender );
		pRender = NULL;
	}
	
	//TODO:此处会阻塞
	//if ( NULL != pWindow )
	//{
	//	SDL_DestroyWindow( pWindow );
	//	pWindow = NULL;
	//}
}	
