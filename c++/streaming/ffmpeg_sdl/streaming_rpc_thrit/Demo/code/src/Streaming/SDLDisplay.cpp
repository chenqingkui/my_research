//#include "stdafx.h"
#include "SDLDisplay.h"
#include "sdl/SDL.h"
#include "sdl/SDL_thread.h"
#include <stdio.h>
#include <string.h>
#ifndef NULL 
#define NULL 0
#endif
#include "UsedDef.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int (*PauseFunc)(void* pParam);
int (*PlayFunc)(void* pParam);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//此函数用于加载图像
SDL_Texture* SDLPlayLoadImage(const char* pszFileName,SDL_Renderer *rend);
void SDLPlayApplySurface(int x, int y, SDL_Texture *tex, SDL_Renderer *rend);
int	 SDLPlayGetTextureSize(SDL_Texture *tex,int* width,int* height);
bool SDLPlayIsPointInRect(int x,int y,SDLPlayRect rect);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void* CreateSDLPlay(void* pWinWnd,int winWidth,int winHeight,int pixelWidth,int pixelHeight)
{
	int nStatus = 0;
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) 
	{  
		printf("SDL_Init fail\n");  
		return NULL;
	} 

	SDL_Window* pWindow = NULL; 
	if(pWinWnd == NULL)
	{
#ifdef SDL_USE_BUTTON
		pWindow = SDL_CreateWindow("SDL2.0 Player",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,winWidth, winHeight+40,SDL_WINDOW_OPENGL);
#else
		pWindow = SDL_CreateWindow("SDL2.0 Player",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,winWidth, winHeight,SDL_WINDOW_OPENGL);
#endif
		if(!pWindow)
		{  
			printf("SDL_CreateWindow fail:%s\n",SDL_GetError());  
			return NULL;
		}
	}
	else
	{
		pWindow = SDL_CreateWindowFrom(pWinWnd);
		if(!pWindow)
		{  
			printf("SDL_CreateWindow fail:%s\n",SDL_GetError());  
			return NULL;
		}
	}
	SDLPlaySrv* pSrv = (SDLPlaySrv*)malloc(sizeof(SDLPlaySrv));
	if(pSrv == NULL)
	{
		return  NULL;
	}

	SDL_Renderer* pSdlRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);  
	if(pSdlRenderer == NULL)
	{
		printf("SDL_CreateRenderer fail:%s\n",SDL_GetError());  
		return NULL;
	}
	SDL_SetRenderDrawColor(pSdlRenderer,255,255,255,255);
	/*
	nStatus = SDL_RenderSetLogicalSize(pSdlRenderer,width,height);
	if(nStatus != 0)
	{
	printf("SDL_RenderSetLogicalSize fail:%s\n",SDL_GetError());
	}*/
	SDL_Texture* pSdlTexture = SDL_CreateTexture(pSdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,pixelWidth,pixelHeight);

	SDLPlayRect playTexRect;
	memset(&playTexRect,0,sizeof(playTexRect));

	SDL_Texture* pPlayBtnTex = NULL;
#ifdef SDL_USE_BUTTON
	pPlayBtnTex = SDLPlayLoadImage("play.bmp",pSdlRenderer);
	if(pPlayBtnTex != NULL)
	{
		playTexRect.x = 0;
		playTexRect.y = winHeight;
		SDLPlayApplySurface(playTexRect.x,playTexRect.y,pPlayBtnTex,pSdlRenderer);
		SDLPlayGetTextureSize(pPlayBtnTex,&(playTexRect.width),&(playTexRect.height));

	}
#endif
	SDLPlayRect pauseTexRect;
	memset(&pauseTexRect,0,sizeof(pauseTexRect));

	SDL_Texture* pPauseBtnTex = NULL;
#ifdef SDL_USE_BUTTON
	pPauseBtnTex = SDLPlayLoadImage("pause.bmp",pSdlRenderer);
	if(pPlayBtnTex != NULL)
	{
		pauseTexRect.x = 50;
		pauseTexRect.y = winHeight;
		SDLPlayApplySurface(pauseTexRect.x,pauseTexRect.y,pPauseBtnTex,pSdlRenderer);
		SDLPlayGetTextureSize(pPauseBtnTex,&(pauseTexRect.width),&(pauseTexRect.height));	
	}
#endif
	if(pWindow != NULL)
	{
		SDL_ShowWindow(pWindow);
	}
	pSrv->magic = 0x86107789;
	pSrv->winWidth = winWidth;
	pSrv->winHeight = winHeight;
	pSrv->pixelWidth = pixelWidth;
	pSrv->pixelHeight = pixelHeight;
	pSrv->pWindow = pWindow;
	pSrv->pRender = pSdlRenderer;
	pSrv->pTexture = pSdlTexture;
	pSrv->pPlayTexture = pPlayBtnTex;
	pSrv->pPauseTexture = pPauseBtnTex;
	pSrv->playTexRect  = playTexRect;
	pSrv->pauseTexRect = pauseTexRect;
	return  pSrv;
}
int SDLDisplayYUV420P(void* pPlay,void* pData[3],int nLineSize[3],int width,int height)
{
	if(pPlay == NULL)
	{
		return -1;
	}

	SDLPlaySrv* pSrv  = (SDLPlaySrv*)pPlay;
	SDL_Renderer* pSdlRenderer = (SDL_Renderer*)pSrv->pRender;

	if(pSdlRenderer == NULL)
	{
		return -1;
	}

	SDL_Texture* pSdlTexture = (SDL_Texture*)pSrv->pTexture;

	if(pSdlTexture == NULL)
	{
		return -1;
	}
	int winW = pSrv->winWidth;
	int winH = pSrv->winHeight;
	int pixelW = pSrv->pixelWidth;
	int pixelH = pSrv->pixelHeight;
	//SDL---------------------------
	SDL_UpdateYUVTexture(pSdlTexture, NULL, (Uint8*)pData[0], nLineSize[0], (Uint8*)pData[1], nLineSize[1], (Uint8*)pData[2], nLineSize[2]);  
	//SDL_UpdateTexture(pSdlTexture, NULL, (Uint8*)pData[0], nLineSize[0]); 
	SDL_RenderClear( pSdlRenderer );  
	SDL_Rect sdlSrcRect;  
	sdlSrcRect.x = 0;  
	sdlSrcRect.y = 0;  
	sdlSrcRect.w = pixelW;  
	sdlSrcRect.h = pixelH; 
	SDL_Rect sdlDstRect;  
	sdlDstRect.x = 0;  
	sdlDstRect.y = 0;  
	sdlDstRect.w = winW;  
	sdlDstRect.h = winH;  
	SDL_RenderCopy( pSdlRenderer, pSdlTexture, NULL, &sdlDstRect );
	//SDL_RenderCopy( pSdlRenderer, pSdlTexture, NULL, NULL );
#ifdef SDL_USE_BUTTON
	SDL_Texture* pPlayBtnTex = (SDL_Texture*)pSrv->pPlayTexture;
	if(pPlayBtnTex != NULL)
	{
		SDLPlayApplySurface(pSrv->playTexRect.x,pSrv->playTexRect.y,pPlayBtnTex,pSdlRenderer);
	}
	SDL_Texture* pPauseBtnTex = (SDL_Texture*)pSrv->pPauseTexture;
	if(pPauseBtnTex != NULL)
	{
		SDLPlayApplySurface(pSrv->pauseTexRect.x,pSrv->pauseTexRect.y,pPauseBtnTex,pSdlRenderer);
	}
#endif
	SDL_RenderPresent( pSdlRenderer );  
	//SDL End-----------------------
	return 0;
}
int SDLWorker(void* pSdlSrv,void* pUserData)
{
	SDLPlaySrv* pSdlPlaySrv = (SDLPlaySrv*)pSdlSrv;
	SDL_Event event;
	void* pUserDef = pUserData;
	for (;;)
	{
		//Wait
		SDL_WaitEvent(&event);
		if(event.type == SDL_QUIT)
		{
			break;
		}
		else if(event.type == SDL_KEYDOWN)
		{
			if(event.key.keysym.sym == SDLK_SPACE)
			{
				PauseFunc(pUserDef);
			}
			else if(event.key.keysym.sym == SDLK_RETURN)
			{
				PlayFunc(pUserDef);
			}
		}
		else if(event.type == SDL_MOUSEBUTTONDOWN)
		{
#ifdef SDL_USE_BUTTON
			int mouseX = event.button.x;
			int mouseY = event.button.y;
			if(pSdlPlaySrv != NULL)
			{
				if(SDLPlayIsPointInRect(mouseX,mouseY,pSdlPlaySrv->playTexRect))
				{
					PlayFunc(pUserDef);
				}
				else if(SDLPlayIsPointInRect(mouseX,mouseY,pSdlPlaySrv->pauseTexRect))
				{
					PauseFunc(pUserDef);
				}
			}
#endif
		}
	}
	printf("SDL quit\n");
	return 0;
}
int   RegistSDLEvent(void* pSrv,const char* pCmd,int (*PFunc)(void* pParam))
{
	int result = -1;
	if(strcmp(pCmd,"Pause") == 0)
	{
		PauseFunc = PFunc;
		result = 0;
	}
	else if(strcmp(pCmd,"Play") == 0)
	{
		PlayFunc = PFunc;
		result = 0;
	}
	return result;
}
int	  DestroySDLPlay(void** ppSrv)
{
	if(ppSrv == NULL)
	{
		return -1;
	}
	SDLPlaySrv* pPlaySrv  = (SDLPlaySrv*)*ppSrv;
	if(pPlaySrv->magic != 0x86107789)
	{
		return -1;
	}


	if(pPlaySrv->pRender != NULL)
	{
		SDL_DestroyRenderer((SDL_Renderer*)(pPlaySrv->pRender));
		pPlaySrv->pRender = NULL;
	}
	if(pPlaySrv->pTexture != NULL)
	{
		SDL_DestroyTexture((SDL_Texture*)(pPlaySrv->pTexture));
		pPlaySrv->pTexture = NULL;
	}
	if(pPlaySrv->pPlayTexture != NULL)
	{
		SDL_DestroyTexture((SDL_Texture*)(pPlaySrv->pPlayTexture));
		pPlaySrv->pPlayTexture = NULL;
	}
	if(pPlaySrv->pPauseTexture != NULL)
	{
		SDL_DestroyTexture((SDL_Texture*)(pPlaySrv->pPauseTexture));
		pPlaySrv->pPauseTexture = NULL;
	}
	if(pPlaySrv->pWindow != NULL)
	{
		SDL_DestroyWindow((SDL_Window*)(pPlaySrv->pWindow));
		pPlaySrv->pWindow  = NULL;
	}

	free(pPlaySrv);
	*ppSrv = NULL;

	//sdl quit
	SDL_Quit();
	return 0;

}
SDL_Texture* SDLPlayLoadImage(const char* pszFileName,SDL_Renderer* pRender)
{
	if(pszFileName == NULL)
	{
		return NULL;
	}
	SDL_Surface *loadedImage = NULL;
	SDL_Texture *texture = NULL;

	loadedImage = SDL_LoadBMP(pszFileName);   //加载图层
	if (loadedImage != NULL)
	{
		texture = SDL_CreateTextureFromSurface(pRender, loadedImage);   //将图层转化为纹理
		SDL_FreeSurface(loadedImage);   //释放掉图层
	}
	else
	{
		printf("load image :%s fail\n",pszFileName);
	}
	return texture;
}

//此函数用于将纹理画到渲染器上
void SDLPlayApplySurface(int x, int y, SDL_Texture *tex, SDL_Renderer *rend)
{
	if(tex == NULL || rend == NULL)
	{
		return ;
	}
	SDL_Rect pos;
	//x，y是图片左上角的坐标
	pos.x = x;
	pos.y = y;
	SDL_QueryTexture(tex, NULL, NULL, &pos.w, &pos.h); 

	SDL_RenderCopy(rend, tex, NULL, &pos);   //将纹理tex画到渲染器rend
}

int	 SDLPlayGetTextureSize(SDL_Texture *pTex,int* pWidth,int* pHeight)
{
	if(pTex == NULL || pWidth == NULL || pHeight == NULL )
	{
		return -1 ;
	}
	SDL_QueryTexture(pTex, NULL, NULL, pWidth, pHeight); 
	return 0;
}
bool SDLPlayIsPointInRect(int x,int y,SDLPlayRect rect)
{
	if((x >= rect.x && x <= (rect.x + rect.width)) 
		&& (y >= rect.y && y <= (rect.y + rect.height)))
	{
		return true;
	}
	return false;
}
int   SDLPlaySignalQuitEvent(void* pSdlThread)
{
	SDL_Event event;
	memset(&event,0,sizeof(event));
	event.type = SDL_QUIT;
	SDL_PushEvent(&event);
	if(pSdlThread != NULL)
	{
		int nWaitStatus  = 0;
		SDL_WaitThread((SDL_Thread*)pSdlThread,&nWaitStatus);
	}
	return 0;
}