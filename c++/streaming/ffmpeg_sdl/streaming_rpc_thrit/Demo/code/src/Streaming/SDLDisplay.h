#ifndef _SDL_DISPLAY_H_
#define _SDL_DISPLAY_H_

#undef main

typedef struct SDLPlayRect
{
	int height;
	int width;
	int x;
	int y;
}SDLPlayRect;
//sdl player service
typedef struct SDLPlaySrv
{
	int	  magic;
	int   winWidth;
	int   winHeight;
	int	  pixelWidth;
	int   pixelHeight;
	void* pWindow;
	void* pRender;
	void* pTexture;
	void* pPlayTexture;
	void* pPauseTexture;
	SDLPlayRect playTexRect;
	SDLPlayRect pauseTexRect;
}SDLPlaySrv,*PSDLPlaySrv;

void* CreateSDLPlay(void* pWinWnd,int winWidth,int winHeight,int pixelWidth,int pixelHeight);

int	  SDLDisplayYUV420P(void* pPlay,void* pData[3],int nLineSize[3],int width,int height);

int	  SDLWorker(void* pSdlSrv,void* pUserData);

int	  DestroySDLPlay(void** ppSrv);

int   RegistSDLEvent(void* pSrv,const char* pCmd,int (*PFunc)(void* pParam));


int   SDLPlaySignalQuitEvent(void* pSdlThread);
#endif