#include "stdafx.h"
#include "SDLWindow.h"
#include "resource.h"


SDLWindow::SDLWindow(void)
{
}


SDLWindow::~SDLWindow(void)
{
}

void SDLWindow::create(CWnd *pCwnd)
{
	SDL_Init(SDL_INIT_VIDEO);
	if (SDL_WasInit(SDL_INIT_VIDEO) != 0) {
		TRACE("Video is initialized.\n");
	} else {
		TRACE("Video is not initialized.\n");
	}
	TRACE("SDLWindow create\n");
    //��mfc���ھ������һ��sdl window
    pWindow = SDL_CreateWindowFrom((void *)(pCwnd->GetDlgItem(IDC_VIDEO)->GetSafeHwnd())); 
	TRACE( "SDL_CreateWindowFrom %s\n", SDL_GetError()); 
    sdlRT.h = 720;
    sdlRT.w = 1280;
    sdlRT.x = 0;
    sdlRT.y = 0; 

    dstRT.h = 1280;
    dstRT.w = 720;
    dstRT.x = 0;
    dstRT.y = 0;
 
    int iW = 1280;
    int iH = 720;

   
    //����yuvһ������ռ���ֽ���
    iPitch = iW*SDL_BYTESPERPIXEL(SDL_PIXELFORMAT_YV12);   
 
    int iWidth = 0;
    int iHeight = 0;
    SDL_GetWindowSize( pWindow, &iWidth, &iHeight );
    dstRT.h = iHeight;
    dstRT.w = iWidth;
 
    //��ȡ��ǰ���û�ͼ���� window����3������һ��Ϊd3d���ڶ���Ϊopengl��������Ϊsoftware
    int iii = SDL_GetNumRenderDrivers();
    //������Ⱦ�����ڶ�������Ϊѡ�õĻ�ͼ������0����d3d
    pRender = SDL_CreateRenderer( pWindow, 0, SDL_RENDERER_ACCELERATED );
    TRACE( "SDL_CreateRenderer %s\n", SDL_GetError()); 
    
    SDL_GetRendererInfo(pRender, &info);
 
    SDL_GetRenderDriverInfo(0, &info);    //d3d
    SDL_GetRenderDriverInfo(1, &info);    //opgl
    SDL_GetRenderDriverInfo(2, &info);    //software
 
    char szInfo[256] = {0};
    TRACE( "info.name = %s\n", info.name);
    TRACE( "SDL_GetRendererInfo %s\n", SDL_GetError());
 
    //��������
    pTexture = SDL_CreateTexture( pRender,SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, iW, iH );   
	TRACE("SDLWindow create finish\n");
 
}

void SDLWindow::destory()
{
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
 
 
    if ( NULL != pWindow )
    {
        SDL_DestroyWindow( pWindow );
        pWindow = NULL;
    }

}


void SDLWindow::upData(u_char *yuv)
{
	//int ret = SDL_UpdateTexture( pTexture, &sdlRT, yuv, iPitch );
	//TRACE("SDL_UpdateTexture ret=%d.\n",ret);
	int e = SDL_UpdateYUVTexture(pTexture,  NULL,	yuv, 1280,	yuv + 1280*720, 1280 / 2, yuv + 1280*720 +1280*720/4, 1280 / 2);
    SDL_RenderClear( pRender );
    SDL_RenderCopy( pRender, pTexture, &sdlRT, &dstRT );
    SDL_RenderPresent( pRender );
}
