#include "stdafx.h"
#include "SDLWindow.h"
#include "resource.h"


SDLWindow::SDLWindow(void)
{
}


SDLWindow::~SDLWindow(void)
{
}

void SDLWindow::init(CWnd *pCwnd, int width, int height)
{
	TRACE("SDLWindow create, width=%d, height=%d\n",width,height);
	if (SDL_Init(SDL_INIT_VIDEO)) {
		TRACE("Video is initialized.\n");
	} else {
		TRACE("Video is not initialized.\n");
	}	
    //��mfc���ھ������һ��sdl window
    pWindow = SDL_CreateWindowFrom((void *)(pCwnd->GetDlgItem(IDC_VIDEO)->GetSafeHwnd())); 
	TRACE( "SDL_CreateWindowFrom %s\n", SDL_GetError()); 
  
    sdlRT.w = width;
	sdlRT.h = height;
    sdlRT.x = 0;
    sdlRT.y = 0; 
  
  
    //����yuvһ������ռ���ֽ���
 
    int iWidth = 0;
    int iHeight = 0;
    SDL_GetWindowSize( pWindow, &iWidth, &iHeight );   
    dstRT.w = iWidth;
	dstRT.h = iHeight;
    dstRT.x = 0;
    dstRT.y = 0;
	TRACE("SDL_GetWindowSize, width=%d, height=%d\n",iWidth,iHeight);
 
    //��ȡ��ǰ���û�ͼ���� window����3������һ��Ϊd3d���ڶ���Ϊopengl��������Ϊsoftware
    //int iii = SDL_GetNumRenderDrivers();
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
	pTexture = SDL_CreateTexture( pRender,SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, width, height );   
	TRACE("SDLWindow create finish\n");
 
}

void SDLWindow::release()
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


void SDLWindow::upVideoYUV(u_char *yuv,int width,int height)
{
	int ret = SDL_UpdateTexture( pTexture, NULL, yuv, width );
	TRACE("upVideoYUV widht=%d, height=%d.\n",width, height);
	//int e = SDL_UpdateYUVTexture(pTexture,  NULL, yuv, width,	yuv + width*height, width / 2, yuv + width*height +width*height/4, width / 2);
    SDL_RenderClear( pRender );
    SDL_RenderCopy( pRender, pTexture, NULL,NULL);
    SDL_RenderPresent( pRender );
}
