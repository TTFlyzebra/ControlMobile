#include "SDL.h"
#pragma once
class SDLWindow
{
public:
	SDLWindow(void);
	~SDLWindow(void);
	void init(CWnd *pCwnd, int width, int height);
	void release();
	void upVideoYUV(u_char *yuv, int width, int height);

private:
	SDL_Window * pWindow;
	SDL_Texture * pTexture;
	SDL_Rect sdlRT;
	SDL_Rect dstRT;
	int iPitch;
	SDL_Renderer * pRender;
	SDL_RendererInfo info;
	CWnd *mPtr;
};

